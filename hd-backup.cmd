@echo off
SETLOCAL EnableDelayedExpansion
echo.
echo [1mHomeDing Configuration Backup Utility.[0m
echo.

REM == Default Values ==

set _conffolder=.\configs
set _allDevices=%_conffolder%\hd-devices.txt
set _help=
set _all=

if "%*"=="" ( set _help=1 )

:restart 

REM == Option Values ==

if [%~1]==[--help] ( set _help=1 && shift /1 && goto :restart )
if [%~1]==[-h]     ( set _help=1 && shift /1 && goto :restart )
if [%~1]==[/h]     ( set _help=1 && shift /1 && goto :restart )
if [%~1]==[/?]     ( set _help=1 && shift /1 && goto :restart )

if [%~1]==[--all] ( set _all=1 && shift /1 && goto :restart )

REM == HELP screen ==

if DEFINED _help (
  echo This is a utility to backup the configuration from a device to the local disk.
  echo.
  echo Usage: hd-backup.bat [options] ^<devicename^>
  echo.  -h, --help     Get a brief help on using this tool
  echo.  --all          download config from from all known devices
  echo.
  echo There will be 3 files saved to the [1m%_conffolder%[0m folder:
  echo.  [1m^<devicename^>-sysinfo.json[0m  containing the result from /api/sysinfo
  echo.  [1m^<devicename^>-env.json[0m      containing the file /env.json
  echo.  [1m^<devicename^>-config.json[0m   containing the file /config.json
  echo.
)

set devicename=%~1

rem == batch mode for all devices ==

if DEFINED _all (
  echo [1mBackup from all known devices:[0m
  for /f "tokens=1" %%G in (%_allDevices%) do (
    echo.  http://%%G/
    call hd-backup %%G >nul:
  )
  echo.
  goto :end
)

rem == verify parameters ==

if NOT DEFINED devicename (
  echo [31m--missing ^<devicename^> parameter--[30m
  goto :end
)

rem == device reachable ? ==

ping -n 1 -r 1 -w 1000 "%devicename%" >nul:
IF %ERRORLEVEL% GTR 0 (
  echo [31mDevice not reachable[30m 1>&2
  goto :end
)

rem == execute ==

echo [1mBackup configuration...[0m
echo.  Device name = %devicename%
echo.


echo [1mget sysinfo...[0m
curl --get http://%devicename%/api/sysinfo --retry 1 --connect-timeout 3 -o %_conffolder%/%devicename%-sysinfo.json 2>nul:
IF %ERRORLEVEL% GTR 0 (
  echo [31mDevice not found[30m 1>&2
  goto :end
)

echo [1mget env.json...[0m
curl --get http://%devicename%/env.json -o %_conffolder%/%devicename%-env.json 2>nul:

echo [1mget config.json...[0m
curl --get http://%devicename%/config.json -o %_conffolder%/%devicename%-config.json 2>nul:
echo.

echo [1msaved...[0m
FOR %%F IN (%_conffolder%\%devicename%-*.json) DO (
  echo [37m  %%F[30m
)

echo [1mdone.[0m

goto :end


echo.
echo get /config.json ...
curl --get http://%devicename%/config.json -o %devicename%-config.json

:end

set _conffolder=
set _help=
