@echo off
setlocal
cd /d "%~dp0"
if not exist build mkdir build
echo Building the NodeScape desktop app (needs g++ and raylib from MSYS2 mingw64)...
g++ -std=c++17 -O2 -Wall -Wextra -mwindows src\core\*.cpp src\gui\*.cpp -lraylib -lopengl32 -lgdi32 -lwinmm -o build\NodeScapeApp.exe
if errorlevel 1 (
    echo Build failed.
    pause
    exit /b 1
)
echo Done: build\NodeScapeApp.exe
