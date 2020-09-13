@echo off

if [%1]==[] (
  echo missing device name as parameter
  goto :end
)
set devicename=%1

python.exe %LOCALAPPDATA%\Arduino15\packages\esp8266\hardware\esp8266\2.7.4\tools\espota.py -i %devicename% -p 8266 --auth=123 -f ..\..\_temp\Standard.ino.bin 

:end