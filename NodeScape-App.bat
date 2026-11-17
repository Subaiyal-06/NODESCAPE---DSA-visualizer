@echo off
setlocal
cd /d "%~dp0"
if not exist "build\NodeScapeApp.exe" call "%~dp0build_app.bat"
if exist "build\NodeScapeApp.exe" start "" "%~dp0build\NodeScapeApp.exe"
