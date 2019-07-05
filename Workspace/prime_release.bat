@echo off
xcopy ..\x64\Release\DarkSun.exe Release\DarkSun.exe /y /q /i
echo ***************** LAUNCHING *****************
cd Release
DarkSun.exe
