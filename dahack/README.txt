Calories and BMI - 使用方法

1. 双击 start-server.bat（黑色窗口不要关，关了 server 就停了）
2. 等窗口显示 "Open http://localhost:8080"
3. 浏览器打开 http://localhost:8080

需要 g++（第一次会自动编译 main.cpp 到 build\main.exe）。
没有 g++ 的话运行：winget install BrechtSanders.WinLibs.POSIX.UCRT

文件：
  main.cpp          计算程序（命令行版本，server 把网页上的答案逐行输入给它）
  server.ps1        网页服务器
  start-server.bat  启动 server
  index.html        网页
修改 main.cpp 保存后，网页会在约 2 秒内自动刷新结果。
