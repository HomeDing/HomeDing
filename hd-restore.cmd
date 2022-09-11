@echo off
SETLOCAL EnableDelayedExpansion
echo.
echo [1mHomeDing Configuration Restore Utility.[0m
echo.

REM == Default Values ==

set _conffolder=.\configs
set _devicename=
set _target=
set _list=
set _help=

if "%*"=="" ( set _help=1 )

:restart 

REM == Option Values ==

if [%~1]==[--help] ( set _help=1 && shift /1 && goto :restart )
if [%~1]==[-h] ( set _help=1 && shift /1 && goto :restart )
if [%~1]==[/h] ( set _help=1 && shift /1 && goto :restart )
if [%~1]==[/?] ( set _help=1 && shift /1 && goto :restart )

if [%~1]==[--list] ( set _list=1 && shift /1 && goto :restart )
if [%~1]==[-l]     ( set _list=1 && shift /1 && goto :restart )

REM == HELP screen ==

if DEFINED _help (
  echo This is a utility to restore a configuration backup to a device.
  echo.
  echo Usage: hd-restore ^<devicename^> ^<target-ip-address^|target-hostname^>
  echo.  -h, --help     Get a brief help on using this tool
  echo.  -l, --list     list existing backups only
  echo.
)

set _devicename=%~1

rem == list mode ==

if DEFINED _list (
  echo [1mList all known backups:[0m

  FOR %%F IN (%_conffolder%\*-sysinfo.json) DO (
    set _f=%%~nF
    echo [37m  !_f:-sysinfo=![30m
  )
  echo.
  goto :end
)


rem == verify parameters ==

if NOT DEFINED _devicename (
  echo missing ^<devicename^> as parameter
  goto :end
)

if [%~2]==[] (
  echo missing ^<target^>
  goto :end
)
set _target=%~2

echo [1mrestoring configuration %_devicename% to %_target% [0m

set curlopts=--retry 1 --connect-timeout 1

echo [1mrestore env.json...[0m
curl %curlopts% --form "fileupload=@./configs/%_devicename%-env.json;filename=/env.json" http://%_target%/
IF %ERRORLEVEL% GTR 0 (
  echo [31mDevice not found[30m 1>&2
  goto :end
)

echo [1mrestore config.json...[0m
curl %curlopts% --form "fileupload=@./configs/%_devicename%-config.json;filename=/config.json" http://%_target%/

echo done.

:end

set _conffolder=
set _devicename=
set _list=
set _help=
