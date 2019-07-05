@echo off
xcopy ..\x64\Debug\DarkSun.exe Debug\DarkSun.exe /y /q /i
echo ***************** LAUNCHING *****************
cd Debug
DarkSun.exe
