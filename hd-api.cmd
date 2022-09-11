@echo off
SETLOCAL EnableDelayedExpansion
echo.
echo [1mHomeDing api utility to call services on a device on CLI.[0m
echo.

REM == Default Values ==

set _curlopts=--retry 2
set _help=
set _cmd=

if "%*"=="" ( set _help=1 )

:restart

REM == Option Values ==

if [%~1]==[--help] ( set _help=1 && shift /1 && goto :restart )
if [%~1]==[-h]     ( set _help=1 && shift /1 && goto :restart )
if [%~1]==[/h]     ( set _help=1 && shift /1 && goto :restart )
if [%~1]==[/?]     ( set _help=1 && shift /1 && goto :restart )

set devicename=%~1
set _cmd=%~2

if DEFINED _help (
  echo This utility enables calling a api on the CLI.
  echo.
  echo Usage: hd-api ^<devicename^> ^<command^>
  echo.  -h, --help     Get a brief help on using this tool
  echo.
  echo Commands are: sysinfo, elements, list, state, state, reboot, reset, resetall, cleanweb, scan, connect
  echo.
)

if NOT DEFINED devicename (
  echo [31mmissing device name as parameter[30m
  echo.
  goto :end
)

if NOT defined _cmd (
  echo [31mmissing command as parameter[30m
  echo.
  goto :end
)

rem == execute == 

echo http://%devicename%/api/%_cmd%
curl --get http://%devicename%/api/%_cmd% %_curlopts%
echo.

:end

set devicename=
set _cmd=
