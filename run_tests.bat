@echo off
setlocal enabledelayedexpansion
if not exist build mkdir build
set FILES=
for %%f in (src\core\*.cpp) do set FILES=!FILES! %%f
for %%t in (tests\test_*.cpp) do (
    echo == %%~nt
    g++ -std=c++17 -Wall -Wextra !FILES! %%t -o build\%%~nt.exe
    if errorlevel 1 (
        echo Build failed for %%t
        goto done
    )
    build\%%~nt.exe
)
:done
pause
