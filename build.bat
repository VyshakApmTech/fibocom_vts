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
pause