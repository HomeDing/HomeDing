@echo on

if [%1]==[] (
  echo missing device name as parameter
  goto :end
)
set devicename=%1

if EXIST ".\_temp\minimal.ino.bin"     ( set binfile=".\_temp\minimal.ino.bin" )
if EXIST "..\..\_temp\minimal.ino.bin" ( set binfile="..\..\_temp\minimal.ino.bin" )

echo uploading: %binfile% to device %devicename% ...

python.exe %LOCALAPPDATA%\Arduino15\packages\esp8266\hardware\esp8266\2.7.4\tools\espota.py -i %devicename% -p 8266 --auth=123 -f %binfile% 

:end