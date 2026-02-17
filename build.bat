@echo off
echo Starting clean compilation...

cd /d %~dp0

echo Removing out folder...
if exist out rmdir /s /q out

echo Running launch.bat...
call tools\launch.bat 8910DM_C3_DM_novolte_L610-IN-32-00-OPENCPU debug

echo Running CMake...
cd out\8910DM_C3_DM_novolte_L610-IN-32-00-OPENCPU_debug
cmake ../.. -G Ninja

echo Building with Ninja...
ninja

echo Build complete!

:: Return to original folder
cd /d %~dp0

echo Back to SDK folder: %cd%

:: ============================================
:: GIT CHECK SECTION (ADDED)
:: ============================================

echo.
echo ---------- GIT CHECK ----------
echo Current branch:
git branch --show-current

echo.
echo Git status:
git status

echo.
set /p confirmPush=Do you want to commit and push changes? (y/n): 

if /I "%confirmPush%"=="y" (

    set /p commitMsg=Enter commit message: 

    git add .

    git commit -m "%commitMsg%"

    git push

    echo.
    echo Git push complete!

) else (
    echo.
    echo Git push skipped.
)

pause
