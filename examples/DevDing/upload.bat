@echo off

if [%1]==[] (
  echo missing device name as parameter
  goto :end
)
set devicename=%1

if EXIST "..\..\_temp\DevDing.ino.bin" ( set binfile="..\..\_temp\DevDing.ino.bin" )
if EXIST ".\_temp\DevDing.ino.bin"     ( set binfile=".\_temp\DevDing.ino.bin" )

echo Uploading %binfile%...
python.exe %LOCALAPPDATA%\Arduino15\packages\esp8266\hardware\esp8266\3.0.0\tools\espota.py -i %devicename% -p 8266 --auth=123 -f %binfile% 

:end
