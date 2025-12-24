@echo off
REM Drum Machine Windows Installer Builder
REM This script builds the Release binary and creates the NSIS installer

setlocal enabledelayedexpansion

echo.
echo ======================================
echo Drum Machine - Windows Installer Build
echo ======================================
echo.

REM Check if NSIS is installed
if not exist "C:\Program Files (x86)\NSIS\makensis.exe" (
    echo ERROR: NSIS not found!
    echo Please install NSIS from: https://nsis.sourceforge.io/Download
    echo.
    pause
    exit /b 1
)

REM Check if CMake is installed
where cmake >nul 2>nul
if errorlevel 1 (
    set "cmake=C:\Program Files\CMake\bin\cmake.exe"
    if not exist "!cmake!" (
        echo ERROR: CMake not found!
        echo Please install CMake or add it to PATH
        pause
        exit /b 1
    )
) else (
    set "cmake=cmake"
)

echo [1/3] Building Release binary...
cd build
"!cmake!" --build . --config Release
if errorlevel 1 (
    echo ERROR: Build failed!
    pause
    exit /b 1
)
cd ..

echo.
echo [2/3] Verifying DLL files...
if not exist "build\bin\Release\DrumMachine.exe" (
    echo ERROR: DrumMachine.exe not found in build\bin\Release
    pause
    exit /b 1
)
if not exist "build\bin\Release\SDL2.dll" (
    echo ERROR: SDL2.dll not found - build failed to include dependencies
    pause
    exit /b 1
)

echo.
echo [3/3] Creating NSIS installer...
"C:\Program Files (x86)\NSIS\makensis.exe" installer\DrumMachine.nsi
if errorlevel 1 (
    echo ERROR: NSIS installer creation failed!
    pause
    exit /b 1
)

echo.
echo ======================================
echo SUCCESS! Installer created:
echo   DrumMachine-0.1.0-Windows.exe
echo.
echo Ready for distribution on Gumroad!
echo ======================================
echo.
pause
