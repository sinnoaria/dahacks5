@echo off
rem Starts the web server that connects the page to main.cpp, then open http://localhost:8080
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0server.ps1"
pause
