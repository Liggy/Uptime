@echo off
setlocal
set CWD=%~dp0
set OutDir=%1

goto main

:BuildMUI
set LANG=%1
set LOCALE=%2

if "%3" == "" (
	set BASELOCALE=%2
) else (
	set BASELOCALE=%3
)

if "%NeutralDone%" == "" (
	set NeutralOut=%OutDir%Uptime.dll
	set NeutralDone=1
	del %OutDir%Uptime.dll
) else (
	set NeutralOut=%OutDir%Uptime-Discard.dll
)

if not exist %OutDir%%LANG%\NUL md %OutDir%%LANG%

if exist %Outdir%%lang%\Uptime.dll.mui del %Outdir%%lang%\Uptime.dll.mui
muirct.exe -q %CWD%DoReverseMuiLoc.rcconfig -x %LOCALE% -g %BASELOCALE% %Outdir%Uptime_%LANG%.dll %NeutralOut% %Outdir%%lang%\Uptime.dll.mui
muirct.exe -c %Outdir%Uptime.dll -e %OutDir%%lang%\Uptime.dll.mui
if "%NeutralOut%" == "%OutDir%Uptime-Discard.dll" del %NeutralOut%
exit /b 0


:main
call :BuildMUI en-US 0x0409
call :BuildMUI de-DE 0x0407 0x0409

endlocal