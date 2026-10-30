@echo off
setlocal
echo ========================================================
echo   Windhawk Backup Suite - High-Performance C++ Builder
echo ========================================================

set "COMPILER=C:\Program Files\Windhawk\Compiler\bin\clang++.exe"
if not exist "%COMPILER%" (
    echo [ERROR] Windhawk compiler not found at %COMPILER%
    pause
    exit /b 1
)

set "OUT_EXE=%~dp0WindhawkBackupSuite.exe"
set "DESKTOP_EXE=%USERPROFILE%\Desktop\WindhawkBackupSuite.exe"
set "EMBED_SCRIPT=%~dp0scripts\embed_icon.ps1"

echo [1/3] Compiling and linking standalone binary...
"%COMPILER%" -target x86_64-w64-mingw32 -std=c++17 -O2 -static -mwindows ^
    -I"%~dp0include" ^
    -I"%~dp0src" ^
    "%~dp0src\main.cpp" ^
    -o "%OUT_EXE%" ^
    -lole32 -luuid -lshell32 -lshlwapi -ladvapi32 -ldwmapi -lcomdlg32 -luser32 -lgdi32

if %ERRORLEVEL% neq 0 (
    echo [ERROR] Build failed with exit code %ERRORLEVEL%
    pause
    exit /b %ERRORLEVEL%
)

echo [2/3] Embedding high-resolution application icon...
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%EMBED_SCRIPT%"

echo [3/3] Copying standalone executable to Desktop...
copy /y "%OUT_EXE%" "%DESKTOP_EXE%" >nul

echo.
echo ========================================================
echo   Build Successful!
echo   Single Portable Executable Ready on Desktop:
echo   %DESKTOP_EXE%
echo ========================================================
echo.

pause
