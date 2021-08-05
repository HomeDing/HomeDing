@echo off

if [%1]==[] (
  echo missing device name as parameter
  goto :end
)
set devicename=%1

if EXIST "..\..\_temp\minimal.ino.bin" ( set binfile="..\..\_temp\minimal.ino.bin" )
if EXIST ".\_temp\minimal.ino.bin"     ( set binfile=".\_temp\minimal.ino.bin" )

echo uploading: %binfile% to device %devicename% ...

@REM python.exe %LOCALAPPDATA%\Arduino15\packages\esp8266\hardware\esp8266\3.0.2\tools\espota.py -i %devicename% -p 8266 --auth=123 -f %binfile% 
python.exe C:\Users\Matthias\Projects\Arduino\Sketches\hardware\esp8266com\esp8266\tools\espota.py -i %devicename% -p 8266 --auth=123 -f %binfile% 

:end