@echo off

@REM set ESPTOOLS=%LOCALAPPDATA%\Arduino15\packages\esp8266\hardware\esp8266\3.0.2\tools
set ESPTOOLS=%USERPROFILE%\Projects\Arduino\Sketches\Hardware\esp8266com\esp8266\tools

if [%1]==[] (
  echo missing device name as parameter
  goto :end
)
set devicename=%1

if EXIST "..\..\build\DevDing.ino.bin" ( set binfile="..\..\build\DevDing.ino.bin" )
if EXIST ".\build\DevDing.ino.bin"     ( set binfile=".\build\DevDing.ino.bin" )

echo Uploading %binfile%...
python %ESPTOOLS%\espota.py -d -r -i %devicename% -P 38288 -p 8266 -a 123 -f %binfile% 

:end
