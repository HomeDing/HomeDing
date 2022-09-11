@echo off
SETLOCAL EnableDelayedExpansion
echo.
echo [1mHomeDing Device discover utility.[0m
echo.

REM == Default Values ==

set _service=_homeding._tcp
@REM set _service=_http._tcp
set _conffolder=.\configs
set _allDevices=%_conffolder%\hd-devices.txt
set _tmpFile=%TEMP%\hdd-%RANDOM%.txt
set _save=
set _list=
set _help=

if "%*"=="" ( set _help=1 )

:restart 

REM == Option Values ==

if [%~1]==[--help] ( set _help=1 && shift /1 && goto :restart )
if [%~1]==[-h] ( set _help=1 && shift /1 && goto :restart )
if [%~1]==[/h] ( set _help=1 && shift /1 && goto :restart )
if [%~1]==[/?] ( set _help=1 && shift /1 && goto :restart )

if [%~1]==[--save] ( set _save=1 && shift /1 && goto :restart )
if [%~1]==[-s] ( set _save=1 && shift /1 && goto :restart )

if [%~1]==[--list] ( set _list=1 && shift /1 && goto :restart )
if [%~1]==[-l] ( set _list=1 && shift /1 && goto :restart )

REM == HELP screen ==

if DEFINED _help (
  echo This is a utility to discover devices using the homeding library.
  echo.
  echo Usage: hd-discover [options]
  echo.  -h, --help     Get a brief help on using this tool
  echo.  -l, --list     list found devices
  echo.  -s, --save     save found devices in file `hd-devices`
  echo.
)

where "dns-sd" 2>nul: >nul:
if %ERRORLEVEL% GTR 0 (
  echo [31mBonjour mDNS is not installed on this system.[30m
  echo Please install from ^<https://support.apple.com/kb/DL999^>
  echo.
  goto :end
) 

if "%_list%%_save%"=="" ( goto :end )

echo Searching Arduino HomeDing devices on the local network...

start /B dns-sd -B %_service% >%_tmpFile%
timeout /T 3 /nobreak >nul:
taskkill /FI "IMAGENAME eq dns-sd.exe" /F >nul:
echo.

@REM tasklist /FI "IMAGENAME eq dns-sd.exe"
@REM find "%_service%" <%_tmpFile%

echo [1mfound devices:[0m
for /f "skip=2 tokens=7" %%G in (%_tmpFile%) do (
  echo.  http://%%G/
)
echo.

if DEFINED _save (
  echo adding to %_allDevices%
  for /f "skip=2 tokens=7" %%G in (%_tmpFile%) do (
    echo %%G>>%_allDevices%
  )
  sort /UNIQUE %_allDevices% /O %_allDevices%
  echo.
)

echo [1mdone.[0m

:end

REM == Cleanup ==

del %_tmpFile% 2>nul:
set _conffolder=
set _service=
set _help=
set _tmpFile=

echo.
