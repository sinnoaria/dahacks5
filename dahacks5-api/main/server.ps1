# server.ps1: a small web server that connects the page (dahacks5-frontend/index.html)
# to main.cpp on Windows. main.cpp has a --cli mode for this; without it the
# program still opens the normal GUI.
#
# The page never calculates anything itself. For each run it sends the lines a
# person would type into main.cpp; the server compiles main.cpp with g++ (again
# whenever the file is saved), types those lines into the program, and sends back
# exactly what it printed.
#
# Start it by double-clicking start-server.bat, or from this folder:
#   powershell -ExecutionPolicy Bypass -File server.ps1
# Then open http://localhost:8080 (or the port in the PORT environment variable).

$ErrorActionPreference = 'Stop'

$Here = $PSScriptRoot
$Source = Join-Path $Here 'main.cpp'
$Program = Join-Path $Here 'build\main.exe'   # main.cpp is compiled to this file
# The page, looked for in this order: the frontend folder, the api folder, then next to the server
$PagePaths = @(
    (Join-Path $Here '..\..\dahacks5-frontend\index.html'),
    (Join-Path $Here '..\index.html'),
    (Join-Path $Here 'index.html')
)

$RunTimeoutMs = 5000      # main.cpp keeps asking when it runs out of answers; stop it after this
$OutputLimit = 64KB       # and keep at most this much of what it printed
$InputLimit = 4KB

$Port = 8080
if ($env:PORT -and [int]$env:PORT -gt 0) { $Port = [int]$env:PORT }

# g++ from PATH, or from where winget puts it if this window was opened before the install
function Find-Compiler {
    $found = Get-Command g++ -ErrorAction SilentlyContinue
    if ($found) { return $found.Source }
    $places = @(
        (Join-Path $env:LOCALAPPDATA 'Microsoft\WinGet\Links\g++.exe'),
        'C:\msys64\ucrt64\bin\g++.exe',
        'C:\msys64\mingw64\bin\g++.exe'
    )
    foreach ($place in $places) { if (Test-Path $place) { return $place } }
    $packages = Join-Path $env:LOCALAPPDATA 'Microsoft\WinGet\Packages'
    if (Test-Path $packages) {
        $exe = Get-ChildItem $packages -Recurse -Filter g++.exe -ErrorAction SilentlyContinue | Select-Object -First 1
        if ($exe) { return $exe.FullName }
    }
    return $null
}

# Start a program, type $InputText into it, and collect what it prints (stdout and stderr)
function Invoke-Captured([string]$File, [string[]]$Arguments, [string]$InputText, [int]$TimeoutMs) {
    $info = New-Object System.Diagnostics.ProcessStartInfo
    $info.FileName = $File
    $info.Arguments = ($Arguments | ForEach-Object { '"' + $_ + '"' }) -join ' '
    $info.WorkingDirectory = Split-Path $File
    $info.UseShellExecute = $false
    $info.CreateNoWindow = $true
    $info.RedirectStandardInput = $true
    $info.RedirectStandardOutput = $true
    $info.RedirectStandardError = $true
    # Compiler messages need its own folder on PATH to find its DLLs
    $info.EnvironmentVariables['PATH'] = (Split-Path $File) + ';' + $env:PATH

    # The program's stdin uses the console's input encoding, and when that is UTF-8
    # .NET sends a byte order mark first, which main.cpp would read as part of the
    # first answer. Start it with a UTF-8 encoding that has no byte order mark.
    $consoleEncoding = $null
    try {
        $consoleEncoding = [Console]::InputEncoding
        [Console]::InputEncoding = New-Object System.Text.UTF8Encoding($false)
    } catch {}
    try {
        $process = [System.Diagnostics.Process]::Start($info)
    } finally {
        if ($consoleEncoding) { try { [Console]::InputEncoding = $consoleEncoding } catch {} }
    }
    $errorTask = $process.StandardError.ReadToEndAsync()
    if ($InputText) {
        $bytes = (New-Object System.Text.UTF8Encoding($false)).GetBytes($InputText)
        $process.StandardInput.BaseStream.Write($bytes, 0, $bytes.Length)
    }
    $process.StandardInput.BaseStream.Close()

    # Read in chunks so a program that never stops printing can't fill memory
    $deadline = [DateTime]::UtcNow.AddMilliseconds($TimeoutMs)
    $buffer = New-Object char[] 8192
    $output = New-Object System.Text.StringBuilder
    $timedOut = $false
    while ($true) {
        $left = [int]($deadline - [DateTime]::UtcNow).TotalMilliseconds
        if ($left -le 0) { $timedOut = $true; break }
        $read = $process.StandardOutput.ReadAsync($buffer, 0, $buffer.Length)
        if (-not $read.Wait($left)) { $timedOut = $true; break }
        if ($read.Result -eq 0) { break }
        [void]$output.Append($buffer, 0, $read.Result)
        if ($output.Length -ge $OutputLimit) { $timedOut = $true; break }
    }
    if ($timedOut -and -not $process.HasExited) {
        try { $process.Kill() } catch {}
    }
    [void]$process.WaitForExit(2000)
    $errors = ''
    if ($errorTask.Wait(1000)) { $errors = $errorTask.Result }

    $text = $output.ToString()
    if ($text.Length -gt $OutputLimit) { $text = $text.Substring(0, $OutputLimit) }
    $code = -1
    if ($process.HasExited -and -not $timedOut) { $code = $process.ExitCode }
    return @{ output = $text + $errors; exitCode = $code; timedOut = $timedOut }
}

# Rebuild main.cpp when it is newer than the program. Returns $null when the
# program is ready, or the compiler's messages when the build fails.
$script:FailedSourceTime = $null
$script:FailedMessages = ''
function Build-IfChanged {
    if (-not (Test-Path $Source)) { return "Cannot find main.cpp next to server.ps1 ($Here)." }
    $sourceTime = (Get-Item $Source).LastWriteTimeUtc
    if ((Test-Path $Program) -and (Get-Item $Program).LastWriteTimeUtc -gt $sourceTime) { return $null }
    if ($script:FailedSourceTime -eq $sourceTime) { return $script:FailedMessages }

    $compiler = Find-Compiler
    if (-not $compiler) { return 'Cannot find g++. Install it with: winget install BrechtSanders.WinLibs.POSIX.UCRT' }

    New-Item -ItemType Directory -Force (Split-Path $Program) | Out-Null
    Write-Host 'Building main.cpp...'
    # -include limits: main.cpp uses numeric_limits without including <limits>; MSVC
    # allows that and g++ may not, so the header is added here instead of in main.cpp.
    # -static: main.exe runs without the compiler's DLLs on PATH.
    # -luser32: main.cpp still contains the Win32 GUI, and g++ ignores the
    # MSVC-only #pragma comment(lib, "user32.lib") inside fitnessGui.cpp.
    $result = Invoke-Captured $compiler @('-std=c++17', '-O2', '-static', '-include', 'limits', '-o', $Program, $Source, '-luser32') '' 120000
    if ($result.exitCode -ne 0) {
        $script:FailedSourceTime = $sourceTime
        $script:FailedMessages = if ($result.output) { $result.output } else { 'The build failed.' }
        Write-Host 'Build failed.'
        return $script:FailedMessages
    }
    $script:FailedSourceTime = $null
    Write-Host 'Build finished.'
    return $null
}

function ConvertTo-JsonString([string]$Text) {
    $out = $Text.Replace('\', '\\').Replace('"', '\"').Replace("`n", '\n').Replace("`r", '\r').Replace("`t", '\t')
    $out = [regex]::Replace($out, '[\x00-\x1f]', { param($m) '\u{0:x4}' -f [int][char]$m.Value })
    return '"' + $out + '"'
}

function Send-Response($Context, [int]$Status, [string]$Type, [string]$Body) {
    $bytes = [System.Text.Encoding]::UTF8.GetBytes($Body)
    $response = $Context.Response
    $response.StatusCode = $Status
    $response.ContentType = $Type
    $response.Headers['Cache-Control'] = 'no-store'
    # Lets the page also work when opened straight from disk (file://)
    $response.Headers['Access-Control-Allow-Origin'] = '*'
    $response.ContentLength64 = $bytes.Length
    $response.OutputStream.Write($bytes, 0, $bytes.Length)
    $response.OutputStream.Close()
}

function Handle($Context) {
    $request = $Context.Request
    $path = $request.Url.AbsolutePath

    if ($path -eq '/' -or $path -eq '/index.html') {
        foreach ($candidate in $PagePaths) {
            if (Test-Path $candidate) {
                Send-Response $Context 200 'text/html; charset=utf-8' ([System.IO.File]::ReadAllText($candidate))
                return
            }
        }
        Send-Response $Context 404 'text/plain' "Cannot find dahacks5-frontend\index.html.`n"
    } elseif ($path -eq '/api/version') {
        $version = 0
        if (Test-Path $Source) {
            $version = [long]((Get-Item $Source).LastWriteTimeUtc - [DateTime]'1970-01-01').TotalSeconds
        }
        Send-Response $Context 200 'application/json' ('{"version":' + $version + '}')
    } elseif ($path -eq '/api/run') {
        if ($request.HttpMethod -ne 'POST') {
            Send-Response $Context 405 'text/plain' "POST the lines to type into main.cpp.`n"
            return
        }
        # The body is exactly what gets typed into main.cpp, one answer per line
        $reader = New-Object System.IO.StreamReader($request.InputStream, [System.Text.Encoding]::UTF8)
        $answers = $reader.ReadToEnd()
        if ($answers.Length -gt $InputLimit) { $answers = $answers.Substring(0, $InputLimit) }
        $answers = $answers -replace "`r", ''

        $buildError = Build-IfChanged
        if ($buildError) {
            $body = '{"buildError":' + (ConvertTo-JsonString $buildError) + '}'
        } else {
            $run = Invoke-Captured $Program @('--cli') $answers $RunTimeoutMs
            $body = '{"exitCode":' + $run.exitCode +
                    ',"timedOut":' + ($(if ($run.timedOut) { 'true' } else { 'false' })) +
                    ',"output":' + (ConvertTo-JsonString $run.output) + '}'
        }
        Send-Response $Context 200 'application/json' $body
    } else {
        Send-Response $Context 404 'text/plain' "Not found.`n"
    }
}

$listener = New-Object System.Net.HttpListener
$listener.Prefixes.Add("http://localhost:$Port/")
try {
    $listener.Start()
} catch {
    Write-Host "Cannot listen on port $Port. Is the server already running? $($_.Exception.Message)"
    exit 1
}

$buildError = Build-IfChanged
if ($buildError) { Write-Host $buildError }
Write-Host "Open http://localhost:$Port  (Ctrl+C to stop)"

try {
    while ($listener.IsListening) {
        # Wait in short steps so Ctrl+C can stop the server
        $next = $listener.GetContextAsync()
        while (-not $next.Wait(500)) {}
        $context = $next.Result
        try {
            Handle $context
        } catch {
            Write-Host "Request failed: $($_.Exception.Message)"
            try { Send-Response $context 500 'text/plain' ($_.Exception.Message + "`n") } catch {}
        }
    }
} finally {
    $listener.Stop()
}
