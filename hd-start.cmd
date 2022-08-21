@echo off
SETLOCAL EnableDelayedExpansion
echo.
echo [1mHomeDing start utility to browse to a device.[0m
echo.

REM == Default Values ==

set devicename=
set _help=

if "%*"=="" ( set _help=1 )

:restart

REM == Option Values ==

if [%~1]==[--help] ( set _help=1 && shift /1 && goto :restart )
if [%~1]==[-h]     ( set _help=1 && shift /1 && goto :restart )
if [%~1]==[/h]     ( set _help=1 && shift /1 && goto :restart )
if [%~1]==[/?]     ( set _help=1 && shift /1 && goto :restart )

set devicename=%~1

if DEFINED _help (
  echo This utility enables calling a api on the CLI.
  echo.
  echo Usage: hd-start ^<devicename^>
  echo.  -h, --help     Get a brief help on using this tool
  echo.
)

if NOT DEFINED devicename (
  echo [31mmissing device name as parameter[30m
  echo.
  goto :end
)

rem == execute == 

echo http://%devicename%/
start http://%devicename%/
echo.

:end

set devicename=
set _help=
