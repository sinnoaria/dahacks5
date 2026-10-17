// server.cpp: a small web server for the calorie page (index.html).
//
// The page never calculates anything itself. For each request the server types
// the page's answers into main.cpp, one per line, and sends back exactly what
// main.cpp prints. When main.cpp changes, the next request rebuilds it, and the
// page notices the change within two seconds.
//
// Folders: this file and main.cpp in api/, the page in frontend/index.html.
// Build and start from inside api/:
//   g++ -std=c++17 -O2 -o server server.cpp
//   ./server
// Then open http://localhost:8080 (or the port in the PORT environment variable).

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cctype>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
using namespace std;

const string SOURCE = "main.cpp";
const string PROGRAM = "./calories"; // main.cpp is compiled to this file
// The page, looked for in this order: the frontend folder, then next to the server
const char* PAGE_PATHS[] = {"../frontend/index.html", "index.html"};

// The answers main.cpp reads, in the order it asks for them
const char* ANSWER_ORDER[] = {"weight", "feet", "inches", "distance", "choice"};

// Last modification time of a file, or 0 if it doesn't exist
time_t modifiedTime(const string& path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) return 0;
    return info.st_mtime;
}

bool readFile(const string& path, string& content) {
    ifstream file(path, ios::binary);
    if (!file) return false;
    stringstream buffer;
    buffer << file.rdbuf();
    content = buffer.str();
    return true;
}

// Run a shell command, collect everything it prints, and return its exit code
int runCommand(const string& command, string& output) {
    output.clear();
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) return -1;
    char buffer[4096];
    size_t count;
    while ((count = fread(buffer, 1, sizeof buffer, pipe)) > 0) output.append(buffer, count);
    int status = pclose(pipe);
    if (status == -1 || !WIFEXITED(status)) return -1;
    return WEXITSTATUS(status);
}

// Rebuild main.cpp when it is newer than the program. Returns an empty string
// when the program is ready, or the compiler's messages when the build fails.
string buildIfChanged() {
    static time_t failedSource = 0;
    static string failedMessages;

    time_t source = modifiedTime(SOURCE);
    if (source == 0) return "Cannot find " + SOURCE + " in the folder the server was started from.";
    if (source < modifiedTime(PROGRAM)) return "";
    if (source == failedSource) return failedMessages;

    cout << "Building " << SOURCE << "..." << endl;
    string messages;
    int code = runCommand("g++ -std=c++17 -O2 -o " + PROGRAM + " " + SOURCE + " 2>&1", messages);
    if (code != 0) {
        failedSource = source;
        failedMessages = messages.empty() ? "The build failed." : messages;
        cout << "Build failed." << endl;
        return failedMessages;
    }
    failedSource = 0;
    cout << "Build finished." << endl;
    return "";
}

// Feed the answers to the program and collect what it prints
int runProgram(const string& answers, string& output) {
    char inputPath[] = "/tmp/calories-input-XXXXXX";
    int fd = mkstemp(inputPath);
    if (fd == -1) {
        output = "Could not create a temporary input file.";
        return -1;
    }
    bool written = write(fd, answers.data(), answers.size()) == (ssize_t)answers.size();
    close(fd);

    int code = -1;
    if (written)
        code = runCommand("timeout 5 " + PROGRAM + " < " + inputPath + " 2>&1", output);
    else
        output = "Could not write the temporary input file.";
    unlink(inputPath);
    return code;
}

string urlDecode(const string& text) {
    string out;
    for (size_t i = 0; i < text.size(); ++i) {
        if (text[i] == '+') {
            out += ' ';
        } else if (text[i] == '%' && i + 2 < text.size() &&
                   isxdigit((unsigned char)text[i + 1]) && isxdigit((unsigned char)text[i + 2])) {
            out += (char)stoi(text.substr(i + 1, 2), nullptr, 16);
            i += 2;
        } else {
            out += text[i];
        }
    }
    return out;
}

map<string, string> parseQuery(const string& query) {
    map<string, string> params;
    stringstream pairs(query);
    string pair;
    while (getline(pairs, pair, '&')) {
        size_t eq = pair.find('=');
        string key = urlDecode(pair.substr(0, eq));
        params[key] = eq == string::npos ? "" : urlDecode(pair.substr(eq + 1));
    }
    return params;
}

// One answer per line: drop any line breaks and keep it short
string oneLine(const string& value) {
    string out;
    for (char c : value)
        if (c != '\n' && c != '\r') out += c;
    return out.substr(0, 40);
}

string jsonString(const string& text) {
    string out = "\"";
    for (unsigned char c : text) {
        if (c == '"') {
            out += "\\\"";
        } else if (c == '\\') {
            out += "\\\\";
        } else if (c == '\n') {
            out += "\\n";
        } else if (c == '\r') {
            out += "\\r";
        } else if (c == '\t') {
            out += "\\t";
        } else if (c < 0x20) {
            char escaped[8];
            snprintf(escaped, sizeof escaped, "\\u%04x", c);
            out += escaped;
        } else {
            out += (char)c;
        }
    }
    return out + "\"";
}

// The first block of // comment lines in main.cpp; the page shows it as the method
string headerComment() {
    string source;
    if (!readFile(SOURCE, source)) return "";
    stringstream lines(source);
    string line, block;
    bool inBlock = false;
    while (getline(lines, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        size_t start = line.find_first_not_of(" \t");
        bool isComment = start != string::npos && line.compare(start, 2, "//") == 0;
        if (!isComment) {
            if (inBlock) break;
            continue;
        }
        string text = line.substr(start + 2);
        if (!text.empty() && text[0] == ' ') text.erase(0, 1);
        block += text + "\n";
        inBlock = true;
    }
    return block;
}

void sendResponse(int client, const string& status, const string& type, const string& body) {
    string response = "HTTP/1.1 " + status + "\r\n" +
                      "Content-Type: " + type + "\r\n" +
                      "Content-Length: " + to_string(body.size()) + "\r\n" +
                      "Cache-Control: no-store\r\n" +
                      "Connection: close\r\n\r\n" + body;
    size_t sent = 0;
    while (sent < response.size()) {
        ssize_t count = send(client, response.data() + sent, response.size() - sent, 0);
        if (count <= 0) break;
        sent += count;
    }
}

string readRequest(int client) {
    string request;
    char buffer[4096];
    while (request.find("\r\n\r\n") == string::npos && request.size() < 16384) {
        ssize_t count = recv(client, buffer, sizeof buffer, 0);
        if (count <= 0) break;
        request.append(buffer, count);
    }
    return request;
}

void handle(int client) {
    istringstream requestLine(readRequest(client));
    string method, target;
    requestLine >> method >> target;

    string path = target, query;
    size_t mark = target.find('?');
    if (mark != string::npos) {
        path = target.substr(0, mark);
        query = target.substr(mark + 1);
    }

    if (method != "GET") {
        sendResponse(client, "405 Method Not Allowed", "text/plain", "Only GET is supported.\n");
    } else if (path == "/" || path == "/index.html") {
        string page;
        bool found = false;
        for (const char* candidate : PAGE_PATHS) {
            if (readFile(candidate, page)) {
                found = true;
                break;
            }
        }
        if (found)
            sendResponse(client, "200 OK", "text/html; charset=utf-8", page);
        else
            sendResponse(client, "404 Not Found", "text/plain", "Cannot find ../frontend/index.html.\n");
    } else if (path == "/api/version") {
        string body = "{\"version\":" + to_string((long long)modifiedTime(SOURCE)) +
                      ",\"comment\":" + jsonString(headerComment()) + "}";
        sendResponse(client, "200 OK", "application/json", body);
    } else if (path == "/api/run") {
        string buildError = buildIfChanged();
        string body;
        if (!buildError.empty()) {
            body = "{\"buildError\":" + jsonString(buildError) + "}";
        } else {
            map<string, string> params = parseQuery(query);
            string answers;
            for (const char* name : ANSWER_ORDER) answers += oneLine(params[name]) + "\n";
            string output;
            int code = runProgram(answers, output);
            body = "{\"exitCode\":" + to_string(code) + ",\"output\":" + jsonString(output) + "}";
        }
        sendResponse(client, "200 OK", "application/json", body);
    } else {
        sendResponse(client, "404 Not Found", "text/plain", "Not found.\n");
    }
}

int main() {
    signal(SIGPIPE, SIG_IGN);

    int port = 8080;
    if (const char* text = getenv("PORT")) {
        if (atoi(text) > 0) port = atoi(text);
    }

    int server = socket(AF_INET, SOCK_STREAM, 0);
    if (server < 0) {
        perror("socket");
        return 1;
    }
    int reuse = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof reuse);

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);
    if (bind(server, (sockaddr*)&address, sizeof address) < 0) {
        perror("bind");
        return 1;
    }
    if (listen(server, 16) < 0) {
        perror("listen");
        return 1;
    }

    string buildError = buildIfChanged();
    if (!buildError.empty()) cout << buildError << endl;
    cout << "Open http://localhost:" << port << "  (Ctrl+C to stop)" << endl;

    while (true) {
        int client = accept(server, nullptr, nullptr);
        if (client < 0) continue;
        timeval timeout{};
        timeout.tv_sec = 5;
        setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);
        handle(client);
        close(client);
    }
}
