@echo off
echo.
echo [1mHomeDing Configuration backup utility.[0m
echo.

set _conffolder=.\configs
set _help=

set _help=
if [%1]==[--help] ( set _help=1 & shift /1 )
if [%1]==[-h] ( set _help=1 & shift /1 )
if [%1]==[] ( set _help=1 )

if DEFINED _help (
  echo This is a utility to download the configuration from a device to the local [1mconfigs[0m folder
  echo.
  echo Usage: hd-backup.bat [parameters] ^<devicename^>
  echo.  -h, --help     Get a brief help on using this tool
  echo.
  echo There will be 3 files saved to the folder:
  echo.  [1m^<devicename^>-sysinfo.json[0m  containing the result from /api/sysinfo
  echo.  [1m^<devicename^>-env.json[0m      containing the file /env.json
  echo.  [1m^<devicename^>-config.json[0m   containing the file /config.json
  echo.
)


set devicename=%1


rem == verify parameters == 

if NOT DEFINED devicename (
  echo [31m--missing ^<devicename^> as parameter 1--[30m
  goto :end
)


rem == execute == 

echo [1mDownload...[0m
echo.  Device name = %devicename%
echo.

if NOT EXIST %_conffolder% (
  md "%_conffolder%"
)

echo [1mPing...[0m
ping %devicename% /nr 1 | find "TTL"
if ERRORLEVEL 1 (
  echo [31mDevice not found[30m
  goto :end
)
echo.

echo [1mget sysinfo...[0m
curl --get http://%devicename%/api/sysinfo -o %_conffolder%/%devicename%-sysinfo.json 2>nul:

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
