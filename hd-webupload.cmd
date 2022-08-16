@echo off
SETLOCAL EnableDelayedExpansion
echo.
echo [1mHomeDing upload utility uploading web files to device.[0m
echo.

REM == Default Values ==

set _webfolder=.\examples\standard\data
set _curlopts=--retry 2
set _folder=dist
set _help=
set _clean=

if "%*"=="" ( set _help=1 )

:restart

REM == Option Values ==

if [%~1]==[--help] ( set _help=1 && shift /1 && goto :restart )
if [%~1]==[-h]     ( set _help=1 && shift /1 && goto :restart )
if [%~1]==[/h]     ( set _help=1 && shift /1 && goto :restart )
if [%~1]==[/?]     ( set _help=1 && shift /1 && goto :restart )

if [%~1]==[-c]      ( set _clean=1 && shift /1 && goto :restart )
if [%~1]==[--clean] ( set _clean=1 && shift /1 && goto :restart )

set devicename=%~1

if DEFINED _help (
  echo This utility enables uploading files from a folder into the filesystem of a device
  echo using http uploads.
  echo.
  echo To create the distribution files use [1mnpm run build[0m and [1mnpm run pack[0m
  echo.
  echo Usage: hd-webupload.bat [options] ^<devicename^> [example]
  echo.  -h, --help     Get a brief help on using this tool
  echo.  -c, --clean    Clean existing files on device before upload
  echo.
  echo When no example parameter is given the files from the [1mstandard[0m data folder are used.
  echo.
)

if NOT DEFINED devicename (
  echo.
  echo [31mmissing device name as parameter[30m
  goto :end
)

if NOT [%~2]==[]  (
  set _webfolder=.\examples\%~2\data
)

rem == execute == 

if DEFINED _clean (
  echo [1mCleaning...[0m
  curl http://%devicename%/api/cleanweb
)

echo [1mUploading...[0m
echo.  Device name = %devicename%
echo.  Folder      = %_webfolder%
echo.

FOR %%F IN (%_webfolder%\*.*) DO (
  echo [37m%%F[30m
  curl --form "fileupload=@%%F;filename=/%%~nxF" http://%devicename%/ %_curlopts%
)

FOR %%F IN ("%_webfolder%\i\*.svg") DO (
  echo [37m%%F[30m
  curl --form "fileupload=@%%F;filename=/i/%%~nxF" http://%devicename%/ %_curlopts%
)

echo.
echo http://%devicename%/ updated.

:end

set _folder=
echo.

