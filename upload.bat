@echo off

if EXIST "%LOCALAPPDATA%\Arduino15\packages\esp8266\hardware\esp8266\3.0.2\tools" ( set ESPTOOLS="%LOCALAPPDATA%\Arduino15\packages\esp8266\hardware\esp8266\3.0.2\tools")
if EXIST "%USERPROFILE%\Projects\Arduino\Sketches\Hardware\esp8266com\esp8266\tools" ( set ESPTOOLS="%USERPROFILE%\Projects\Arduino\Sketches\Hardware\esp8266com\esp8266\tools")
set ESPTOOLS

if [%1]==[] (
  echo missing device name as parameter
  goto :end
)
set devicename=%1

if EXIST ".\build\DevDing.ino.bin"     ( set binfile=".\build\DevDing.ino.bin" )
if EXIST ".\build\Radio.ino.bin"     ( set binfile=".\build\Radio.ino.bin" )
if EXIST ".\build\minimal.ino.bin"     ( set binfile=".\build\minimal.ino.bin" )

echo uploading: %binfile% to device %devicename% ...
python %ESPTOOLS%\espota.py -d -r -i %devicename% -P 38288 -p 8266 -a 123 -f %binfile% 

:end
