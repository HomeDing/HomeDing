@echo off
SETLOCAL EnableDelayedExpansion

echo.
echo [1mHomeDing arduino-cli build tool for Windows.[0m
echo.

REM == Default Values ==
@REM https://arduino.github.io/arduino-cli/0.36/sketch-project-file/

set _hd-sketch=
set _hd-compile=1
set _hd-upload=1
set _hd-verbose=
set _hd-compile-level=fatal
set _hd-case=

set _hd-fqbn=esp32:esp32:esp32c3
set _hd-properties="JTAGAdapter=default,CDCOnBoot=cdc,PartitionScheme=default,CPUFreq=160,FlashMode=qio,FlashFreq=80,FlashSize=4M,UploadSpeed=921600,DebugLevel=none,EraseFlash=none"
set _hd-cachedir="%USERPROFILE%\AppData\Local\Temp\arduino"
set _hd-warnings=none
set _hd-logfile="./build.log"
set _hd-port=COM10
set _hd-pass=123
set _help=

@REM _hd-warnings=none, default, more, all

if "%*"=="" ( set _help=1 )

:restart 

REM == Option Values ==

if [%~1]==[--help] ( set _help=1 && shift /1 && goto :restart )
if [%~1]==[-h] ( set _help=1 && shift /1 && goto :restart )
if [%~1]==[/h] ( set _help=1 && shift /1 && goto :restart )
if [%~1]==[/?] ( set _help=1 && shift /1 && goto :restart )

if [%~1]==[-v]        ( set _hd-verbose=1   && shift /1 && goto :restart )
if [%~1]==[-c]        ( set "_hd-upload="   && set _hd-compile=1   && shift /1 && goto :restart )
if [%~1]==[-compile]  ( set "_hd-upload="   && set _hd-compile=1   && shift /1 && goto :restart )
if [%~1]==[-u]        ( set "_hd-compile="  && set _hd-upload=1    && shift /1 && goto :restart )
if [%~1]==[-upload]   ( set "_hd-compile="  && set _hd-upload=1    && shift /1 && goto :restart )
if [%~1]==[-cu]       ( set _hd-compile=1   && set _hd-upload=1    && shift /1 && goto :restart )

if [%~1]==[-fqbn]     ( set _hd-fqbn=%~2 && shift /1  && shift /1 && goto :restart )
if [%~1]==[-port]     ( set _hd-port=%~2 && shift /1  && shift /1 && goto :restart )
if [%~1]==[--pass]    ( set _hd-pass=%~2 && shift /1  && shift /1 && goto :restart )

if [%~1]==[--case] (
  set _hd-case=%~2
  shift /1
  shift /1
  call hd-build-%~2.bat >> %_hd-logfile%
  goto :restart
 )

@REM if [%~1]==[-p] 
@REM --upload-field password=abc

REM == HELP screen ==

if DEFINED _help (
  echo This is a utility to compile and upload a sketch using arduino-cli
  echo.
  echo Usage: hd-build [options] ^<sketchname^>
  echo.  -h, --help     Get a brief help on using this tool
  echo.  -v             verbose compile
  echo.  -cu            compile and upload sketch = default
  echo.  -c, --compile  compile only
  echo.  -u, --upload   upload only
  echo.  --case <cname> use variables from hd-build-<cname>.bat 
  echo.
  echo. Example:  .\hd-build --case sc01 standard.ino
  echo.
)

if [%~1] NEQ [] ( set _hd-sketch=%~1 )

@REM check for board updates
@REM arduino-cli core upgrade

@echo HomeDing Build %_hd-sketch% ... > %_hd-logfile%
@echo. >> %_hd-logfile%


@REM take values from a case file 
if NOT DEFINED _hd-case (
  echo "No case specified."
  goto :end
)


if DEFINED _hd-verbose (
  set _hd-compile-level=debug

  @echo.>> %_hd-logfile%
  set | find "_hd-" >> %_hd-logfile%
  @echo.>> %_hd-logfile%
) 

set | find "_hd-"

if DEFINED _hd-compile (
  @echo [1mCompiling %_hd-sketch% ...[0m
  @echo. 

  @echo.>> %_hd-logfile%
  @echo.Compiling %_hd-sketch% ... >> %_hd-logfile%


  echo using...
  echo arduino-cli compile %_hd-sketch% --fqbn %_hd-fqbn% --build-property %_hd-properties% --build-cache-path %_hd-cachedir% --build-path ./build --warnings %_hd-warnings% --dump-profile --log --no-color --log-level %_hd-compile-level%

  arduino-cli compile %_hd-sketch% ^
  --fqbn %_hd-fqbn% --build-property %_hd-properties% ^
  --build-cache-path %_hd-cachedir% ^
  --build-path ./build ^
  --warnings %_hd-warnings% ^
  --log --no-color --log-level %_hd-compile-level% >> %_hd-logfile%
  @REM --dump-profile 
  echo.
)


if DEFINED _hd-upload (
  @echo [1mUploading ./build/%_hd-sketch%.bin ...[0m

  @echo.>> %_hd-logfile%
  @echo.Uploading %_hd-sketch% ... >> %_hd-logfile%

  arduino-cli upload --fqbn %_hd-fqbn% --input-dir ./build --port %_hd-port% ^
  >> %_hd-logfile%
  @REM --upload-field password=%hd_pass% ^
  echo.
)

goto :end

@REM ----- ------ ----- ------ ----- ------

:end
@echo off

set _hd-sketch=
set _hd-compile=
set _hd-upload=
set _hd-verbose=
set _hd-compile-level=
set _hd-fqbn=
set _hd-properties=
set _hd-cachedir=
set _hd-warnings=
set _hd-logfile=
set _hd-port=
set _hd-pass=
set _help=
set _hd-case=

set | find "_hd"

@REM arduino-cli compile BigDisplay.ino --fqbn esp32:esp32:esp32c3 --build-property "JTAGAdapter=default,CDCOnBoot=cdc,PartitionScheme=default,CPUFreq=160,FlashMode=qio,FlashFreq=80,FlashSize=4M,UploadSpeed=921600,DebugLevel=none,EraseFlash=none" --build-path ./build --dump-profile --log-level debug --build-cache-path "%USERPROFILE%\AppData\Local\Temp\arduino"



@REM arduino-cli compile BigDisplay.ino 
@REM   --fqbn esp32:esp32:esp32c3
@REM   --build-property "JTAGAdapter=default,CDCOnBoot=cdc,PartitionScheme=default,CPUFreq=160,FlashMode=qio,FlashFreq=80,FlashSize=4M,UploadSpeed=921600,DebugLevel=none,EraseFlash=none"
@REM   --build-path ./build
@REM   --dump-profile --log-level debug
@REM   --build-cache-path "%USERPROFILE%\AppData\Local\Temp\arduino"

@REM   --warnings %_hd-warnings%
@REM   --log --no-color --log-level %_hd-compile-level% >> %_hd-logfile%

@REM arduino-cli compile BigDisplay.ino --profile lilygo_t_display_s3 `
@REM   --build-property "JTAGAdapter=default,LoopCore=1,EventsCore=1,USBMode=hwcdc,CDCOnBoot=cdc,MSCOnBoot=default,DFUOnBoot=default,UploadMode=default,PartitionScheme=app3M_fat9M_16MB,DebugLevel=none,EraseFlash=none" `
@REM   --build-path ./build --dump-profile --log-level debug `
@REM   --build-cache-path "%USERPROFILE%\AppData\Local\Temp\arduino"

@REM   --warnings %_hd-warnings%
@REM   --log --no-color --log-level %_hd-compile-level% >> %_hd-logfile%
