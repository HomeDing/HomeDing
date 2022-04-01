@echo off

set ESPTOOLS=%LOCALAPPDATA%\Arduino15\packages\esp8266\hardware\esp8266\3.0.2\tools

if [%1]==[] (
  echo missing device name as parameter
  goto :end
)
set devicename=%1

if EXIST "..\..\_temp\Probe.ino.bin" ( set binfile="..\..\_temp\Probe.ino.bin" )
if EXIST ".\_temp\Probe.ino.bin"     ( set binfile=".\_temp\Probe.ino.bin" )

echo uploading: %binfile% to device %devicename% ...
python %ESPTOOLS%\espota.py -d -r -i %devicename% -P 38288 -p 8266 -a 123 -f %binfile% 

:end
