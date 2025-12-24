@echo off
REM Build script for Drum Machine
REM Looks for Visual Studio and builds the project

setlocal enabledelayedexpansion

REM Try to find Visual Studio 2022
set VS_PATH=
for /d %%D in ("C:\Program Files\Microsoft Visual Studio\2022\*") do (
    if exist "%%D\VC\Auxiliary\Build\vcvars64.bat" (
        set VS_PATH=%%D
        goto found_vs
    )
)

REM Try to find Visual Studio 2019
for /d %%D in ("C:\Program Files (x86)\Microsoft Visual Studio\2019\*") do (
    if exist "%%D\VC\Auxiliary\Build\vcvars64.bat" (
        set VS_PATH=%%D
        goto found_vs
    )
)

echo ERROR: Visual Studio not found!
exit /b 1

:found_vs
echo Found Visual Studio at: %VS_PATH%
call "%VS_PATH%\VC\Auxiliary\Build\vcvars64.bat"

REM Navigate to build directory
cd /d "%~dp0"

REM Build Release configuration
echo Building Release configuration...
msbuild DrumMachine.sln /p:Configuration=Release /p:Platform=x64 /m

if errorlevel 1 (
    echo Build failed!
    exit /b 1
)

echo Build successful!
echo Binary at: %~dp0bin\Release\DrumMachine.exe
exit /b 0
