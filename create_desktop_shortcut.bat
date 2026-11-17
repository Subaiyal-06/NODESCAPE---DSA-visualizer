@echo off
setlocal
cd /d "%~dp0"
echo Creating Desktop Shortcut for NodeScape...
powershell -ExecutionPolicy Bypass -File "%~dp0create_shortcut.ps1"
echo.
pause
