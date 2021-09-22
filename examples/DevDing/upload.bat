@echo off

set ESPTOOLS=%LOCALAPPDATA%\Arduino15\packages\esp8266\hardware\esp8266\3.0.2\tools

@REM need python 3.x
@REM + pip install pyserial

if [%1]==[] (
  echo missing device name as parameter
  goto :end
)
set devicename=%1

if EXIST "..\..\_temp\DevDing.ino.bin" ( set binfile="..\..\_temp\DevDing.ino.bin" )
if EXIST ".\_temp\DevDing.ino.bin"     ( set binfile=".\_temp\DevDing.ino.bin" )

echo Uploading %binfile%...
python %ESPTOOLS%\espota.py -d -r -i %devicename% -P 38288 -p 8266 -a 123 -f %binfile% 

:end
