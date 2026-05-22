@echo off
echo ========================================
echo  Finalizer - Build Script (Windows)
echo ========================================
echo.

:: Try common devkitPro install locations
set "DKP="
if exist "C:\devkitPro\msys2\usr\bin\bash.exe" set "DKP=C:\devkitPro"
if exist "D:\devkitPro\msys2\usr\bin\bash.exe" set "DKP=D:\devkitPro"

:: If not found in common locations, search via registry
if "%DKP%"=="" (
    for /f "tokens=2*" %%a in ('reg query "HKLM\SOFTWARE\devkitPro" /v "devkitpro" 2^>nul') do set "DKP=%%b"
)

if "%DKP%"=="" (
    echo ERROR: devkitPro not found.
    echo.
    echo Please install devkitPro first:
    echo   1. Go to https://github.com/devkitPro/installer/releases
    echo   2. Download and run devkitProUpdater-X.X.X.exe
    echo   3. Re-run this script
    pause
    exit /b 1
)

echo devkitPro found at: %DKP%
echo.

set "MSYS2=%DKP%\msys2\usr\bin\bash.exe"
set "PACMAN=%DKP%\tools\bin\dkp-pacman.exe"

:: Check if devkitPSP is already installed
if not exist "%DKP%\devkitPSP\bin\psp-gcc.exe" (
    echo PSP SDK not found. Installing via dkp-pacman...
    echo This will take a few minutes on first run.
    echo.

    "%PACMAN%" -Sy --noconfirm psp-dev

    if %ERRORLEVEL% NEQ 0 (
        echo.
        echo ERROR: PSP SDK installation failed.
        echo Please check your internet connection and try again.
        pause
        exit /b 1
    )
    echo PSP SDK installed successfully.
    echo.
) else (
    echo PSP SDK found. OK
    echo.
)

:: Build using MSYS2 with correct env
echo Building Finalizer...
echo.

"%MSYS2%" -l -c "export DEVKITPRO=/opt/devkitpro && export DEVKITPSP=$DEVKITPRO/devkitPSP && export PSPSDK=$DEVKITPRO/devkitPSP/psp/sdk && export PATH=$PATH:$DEVKITPSP/bin:$DEVKITPRO/tools/bin && cd '$(cygpath '%~dp0')' && make"

if %ERRORLEVEL% == 0 (
    echo.
    echo ========================================
    echo  BUILD SUCCESS!
    echo ========================================
    echo.
    echo Your file is ready: EBOOT.PBP
    echo.
    echo To install on PSP:
    echo   1. Create folder: PSP\GAME\Finalizer\ on your memory stick
    echo   2. Copy EBOOT.PBP into that folder
    echo   3. Place your wallpaper at: PICTURE\bg.jpg on your memory stick
    echo   4. Launch from PSP Game menu
    echo.
    echo The app will:
    echo   - Fix X/O button assignment if on Japanese system
    echo   - Set ms0:/PICTURE/bg.jpg as your XMB wallpaper
    echo   - Set username to "Royal Gamez"
    echo   - Silently delete ARK_cIPL and ARK_Loader from PSP/GAME
) else (
    echo.
    echo ========================================
    echo  BUILD FAILED
    echo ========================================
    echo Check the errors above.
)

pause
