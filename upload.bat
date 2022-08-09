@echo off
echo.
echo [1mHomeDing Firmware OTA upload utility.[0m
echo.

if EXIST "%LOCALAPPDATA%\Arduino15\packages\esp8266\hardware\esp8266\3.0.2\tools" ( set ESPTOOLS="%LOCALAPPDATA%\Arduino15\packages\esp8266\hardware\esp8266\3.0.2\tools")
if EXIST "%USERPROFILE%\Projects\Arduino\Sketches\Hardware\esp8266com\esp8266\tools" ( set ESPTOOLS="%USERPROFILE%\Projects\Arduino\Sketches\Hardware\esp8266com\esp8266\tools")
@REM set ESPTOOLS

if [%1]==[] (
  echo [31m--missing device name as parameter 1--[30m
  goto :end
)
set devicename=%1

if [%2]==[] (
  @REM find last built bin file
  FOR %%F IN (.\build\*.bin) DO (
    echo [37m  %%F[30m
    set binfile=%%F%
  )
) ELSE (
  if EXIST ".\bin\%2.ino.bin" (
    set binfile=.\bin\%2.ino.bin
  )
)

if NOT defined binfile (
  echo [31m--no firmware file found--[30m
  goto :end
)


echo [1m...[0m
echo [1mUploading...[0m
echo [37m  Device name = %devicename%[30m
echo [37m  Firmware    = %binfile%[30m

python %ESPTOOLS%\espota.py -d -r -i %devicename% -P 38288 -p 8266 -a 123 -f %binfile% 

:end
