@echo off
setlocal EnableDelayedExpansion
color 0A
title FrameX Builder - by ZAKI @zg22x

echo.
echo  ===================================================
echo   FrameX v1.0.0 - Build + Installer Script
echo   Developer: ZAKI  ^|  Instagram/TikTok: @zg22x
echo  ===================================================
echo.

:: ===========================================================
::  AUTO-DETECT PATHS
:: ===========================================================

:: --- CMake detection ---
set CMAKE_EXE=
for %%P in (
    "C:\Program Files\CMake\bin\cmake.exe"
    "C:\cmake\bin\cmake.exe"
    "C:\Program Files (x86)\CMake\bin\cmake.exe"
) do (
    if exist %%P set CMAKE_EXE=%%~P
)
:: also try PATH
where cmake >nul 2>&1 && set CMAKE_EXE=cmake

:: --- Qt detection (try common install paths) ---
set QT_DIR=
for %%V in (6.8.0 6.7.3 6.7.2 6.7.1 6.7.0 6.6.3 6.6.0 6.5.0) do (
    for %%C in (msvc2022_64 msvc2019_64) do (
        if exist "C:\Qt\%%V\%%C\bin\qmake.exe" (
            if "!QT_DIR!"=="" set QT_DIR=C:\Qt\%%V\%%C
        )
    )
)

:: --- FFmpeg detection ---
set FFMPEG_DIR=
for %%P in ("C:\ffmpeg" "C:\Program Files\ffmpeg" "%USERPROFILE%\ffmpeg") do (
    if exist "%%~P\lib\avcodec.lib" set FFMPEG_DIR=%%~P
    if exist "%%~P\lib\libavcodec.dll.a" set FFMPEG_DIR=%%~P
)

:: --- Inno Setup detection ---
set INNO_EXE=
for %%P in (
    "C:\Program Files (x86)\Inno Setup 6\ISCC.exe"
    "C:\Program Files\Inno Setup 6\ISCC.exe"
    "C:\InnoSetup\ISCC.exe"
) do (
    if exist %%P set INNO_EXE=%%~P
)

:: ===========================================================
::  PRINT DETECTED PATHS
:: ===========================================================

echo  Detected tools:
echo  ---------------

if defined CMAKE_EXE (
    echo  [OK] CMake       : !CMAKE_EXE!
) else (
    echo  [X] CMake        : NOT FOUND
)

if defined QT_DIR (
    echo  [OK] Qt          : !QT_DIR!
) else (
    echo  [X] Qt           : NOT FOUND
)

if defined FFMPEG_DIR (
    echo  [OK] FFmpeg      : !FFMPEG_DIR!
) else (
    echo  [X] FFmpeg       : NOT FOUND (optional - app will build without it)
)

if defined INNO_EXE (
    echo  [OK] Inno Setup  : !INNO_EXE!
) else (
    echo  [X] Inno Setup   : NOT FOUND (optional - only needed for installer)
)

echo.

:: ===========================================================
::  CHECK CRITICAL TOOLS
:: ===========================================================
set MISSING=0

if not defined CMAKE_EXE (
    set MISSING=1
    echo  [!] CMake is required. Download from:
    echo      https://cmake.org/download/
    echo      Choose "cmake-3.xx.x-windows-x86_64.msi"
    echo      Check "Add CMake to system PATH" during install.
    echo.
)

if not defined QT_DIR (
    set MISSING=1
    echo  [!] Qt Framework is required. Download from:
    echo      https://www.qt.io/download-open-source
    echo      Install Qt 6.7 with "MSVC 2019 64-bit" component.
    echo.
)

if %MISSING%==1 (
    echo  ===================================================
    echo   Please install the missing tools above, then
    echo   run this script again.
    echo  ===================================================
    echo.
    echo  Quick install guide:
    echo    1. Install Visual Studio 2022 Community (free)
    echo       https://visualstudio.microsoft.com/
    echo       - Select: "Desktop development with C++"
    echo.
    echo    2. Install Qt 6.7 (free - open source)
    echo       https://www.qt.io/download-open-source
    echo       - Select: Qt 6.7.x > MSVC 2019 64-bit
    echo.
    echo    3. Install CMake
    echo       https://cmake.org/download/
    echo       - Check "Add to PATH" during install
    echo.
    echo    4. Download FFmpeg (for recording features)
    echo       https://www.gyan.dev/ffmpeg/builds/
    echo       - Download "ffmpeg-release-full-shared.7z"
    echo       - Extract to C:\ffmpeg
    echo.
    echo    5. Install Inno Setup 6 (for .exe installer)
    echo       https://jrsoftware.org/isinfo.php
    echo.
    pause
    exit /b 1
)

:: ===========================================================
::  SETUP PATHS
:: ===========================================================
set BUILD_DIR=..\build
set DEPLOY_DIR=..\deploy
set "PATH=%QT_DIR%\bin;%PATH%"

:: Find Visual Studio
set VS_PATH=
for %%P in (
    "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
    "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"
    "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
    "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
) do (
    if exist %%P set VS_PATH=%%~P
)

if not defined VS_PATH (
    echo  [!] Visual Studio not found. Please install VS 2022 Community:
    echo      https://visualstudio.microsoft.com/
    echo      Select "Desktop development with C++"
    pause
    exit /b 1
)

echo  [OK] Visual Studio: !VS_PATH!
echo.
call "!VS_PATH!" >nul 2>&1

:: ===========================================================
::  STEP 1 - CMAKE CONFIGURE
:: ===========================================================
echo  [1/5] Configuring project with CMake...
echo  ----------------------------------------

if not exist %BUILD_DIR% mkdir %BUILD_DIR%
cd %BUILD_DIR%

set CMAKE_ARGS=-G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="%QT_DIR%"

if defined FFMPEG_DIR (
    set CMAKE_ARGS=%CMAKE_ARGS% -DFFMPEG_DIR="%FFMPEG_DIR%"
)

"!CMAKE_EXE!" .. %CMAKE_ARGS%

if errorlevel 1 (
    echo.
    echo  ERROR: CMake configuration failed.
    echo  Make sure Visual Studio C++ workload is installed.
    cd ..
    pause
    exit /b 1
)
echo  [OK] Configuration successful!
echo.

:: ===========================================================
::  STEP 2 - BUILD
:: ===========================================================
echo  [2/5] Building FrameX in Release mode...
echo  ------------------------------------------

"!CMAKE_EXE!" --build . --config Release --parallel

if errorlevel 1 (
    echo.
    echo  ERROR: Build failed. Check the output above for errors.
    cd ..
    pause
    exit /b 1
)
echo  [OK] Build successful!
echo.
cd ..

:: ===========================================================
::  STEP 3 - DEPLOY QT
:: ===========================================================
echo  [3/5] Deploying Qt runtime DLLs...
echo  ------------------------------------

if not exist %DEPLOY_DIR% mkdir %DEPLOY_DIR%
copy "build\Release\FrameX.exe" "%DEPLOY_DIR%\" /Y >nul

"%QT_DIR%\bin\windeployqt.exe" ^
    --release ^
    --no-translations ^
    --multimedia ^
    --sql ^
    --dir "%DEPLOY_DIR%" ^
    "build\Release\FrameX.exe"

if errorlevel 1 (
    echo  WARNING: windeployqt had errors. Continuing...
)
echo  [OK] Qt DLLs deployed!
echo.

:: ===========================================================
::  STEP 4 - COPY FFMPEG
:: ===========================================================
echo  [4/5] Copying FFmpeg DLLs...
echo  ------------------------------

if defined FFMPEG_DIR (
    for %%F in (avcodec avformat avutil swscale swresample avdevice avfilter) do (
        if exist "%FFMPEG_DIR%\bin\%%F*.dll" (
            copy "%FFMPEG_DIR%\bin\%%F*.dll" "%DEPLOY_DIR%\" /Y >nul
            echo  Copied %%F*.dll
        )
    )
    echo  [OK] FFmpeg DLLs copied!
) else (
    echo  [SKIP] FFmpeg not found - some recording features will be limited.
)
echo.

:: ===========================================================
::  STEP 5 - CREATE INSTALLER
:: ===========================================================
echo  [5/5] Creating installer...
echo  ----------------------------

if defined INNO_EXE (
    if not exist "installer\output" mkdir "installer\output"
    "!INNO_EXE!" "installer\setup.iss"

    if errorlevel 1 (
        echo  WARNING: Installer creation failed. The app is still usable from deploy\
    ) else (
        echo  [OK] Installer created!
    )
) else (
    echo  [SKIP] Inno Setup not found. Creating portable ZIP instead...
    powershell -command "Compress-Archive -Path '%DEPLOY_DIR%\*' -DestinationPath 'FrameX_Portable.zip' -Force"
    echo  [OK] Portable ZIP created: FrameX_Portable.zip
)

:: ===========================================================
::  DONE
:: ===========================================================
echo.
echo  ===================================================
echo   BUILD COMPLETE!
echo  ===================================================
echo.

if defined INNO_EXE (
    if exist "installer\output\FrameX_Setup_v1.0.0.exe" (
        echo   Installer : installer\output\FrameX_Setup_v1.0.0.exe
    )
)
if exist "FrameX_Portable.zip" (
    echo   Portable  : FrameX_Portable.zip
)
if exist "%DEPLOY_DIR%\FrameX.exe" (
    echo   Portable  : %DEPLOY_DIR%\FrameX.exe
)
echo.
echo   Developer : ZAKI
echo   Instagram : @zg22x
echo   TikTok    : @zg22x
echo.
echo  ===================================================
pause
