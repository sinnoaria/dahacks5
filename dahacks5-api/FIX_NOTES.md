# 修复说明（2026-10-17）

这个目录是改通后的 `dahacks5-api`：页面 `index.html` → `server.ps1` → `main.exe` 已经端到端跑通。

## 改动

1. **main/main.cpp**：新增 `--cli` / `--once` 非交互模式。服务端用 `--cli` 运行时，从 stdin 读 4 行（身高、体重、1 或 2、距离或卡路里），按页面能解析的格式打印结果后 `return 0`；不带参数时仍然打开原来的 GUI。
2. **main/server.ps1**：
   - g++ 命令加 `-luser32`（GUI 代码需要）；
   - 运行 `main.exe` 时传 `--cli`，避免弹窗和 5 秒超时；
   - 头部注释同步更新。
3. **main/modules/FitnessResults.h、fitnessUserInput.h、fitnessCalculations.h、cinErrorHandling.h**：把 `using namespace std;` 改为按名引入。否则 MinGW + C++17 下 `std::byte` 会与 Windows SDK 的 `byte` 冲突，编译失败。
4. 本机已安装 g++（WinGet 的 WinLibs GCC 16.2.0），`main/build/main.exe` 已编译好（静态链接）。

## 运行

双击 `main\start-server.bat`，然后打开 http://localhost:8080 。

如果改了 `main\main.cpp`，保存后下一次请求会自动重新编译（需要 g++ 在 PATH 中；安装后重开终端）。

## 已验证

- `GET /` → 200，返回 `dahacks5-frontend/index.html`。
- `GET /api/version` → 200，返回 main.cpp 的 mtime。
- `POST /api/run`（68 / 150 / 1 / 3.1）→ `{"exitCode":0,"timedOut":false,...}`，输出 `Calories burned: 353.4`。
- `POST /api/run`（68 / 150 / 2 / 300）→ `Distance needed: 2.63158 miles`。
- 无头浏览器打开页面：大字 353.4，Height/Weight/BMI/Distance 统计和终端输出都正常。

## 注意

- 只有这个目录（server 实际编译的副本）被修改；`dahacks5-backend`、`dahacks5-frontend`、`dahacks5-main` 里的 `main.cpp` 是另外的拷贝，未动。
- 网页每次输入都会运行一次 `main.exe`，不会写 `fitness_results.txt`；要保存结果用 GUI 的 “Save Results”。
- `main\server.cpp` 是旧的 Linux 版本，`start-server.bat` 不使用它。
