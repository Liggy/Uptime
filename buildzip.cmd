@echo off
if not exist %~dp0Dist\nul (
	md %~dp0Dist
	pushd %~dp0dist
) else (
	pushd %~dp0dist
	del uptime_arm64.zip uptime_x64.zip uptime_x86.zip 2> nul
)
cd %~dp0ARM64\Release
zip -R ..\..\dist\uptime_arm64.zip uptime.exe *.mui
cd %~dp0x64\Release
zip -R ..\..\dist\uptime_x64.zip uptime.exe *.mui
cd %~dp0Release
zip -R ..\dist\uptime_x86.zip uptime.exe *.mui
popd
