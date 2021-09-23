@echo off

if [%1]==[] (
  echo missing device name as parameter
  goto :end
)
set devicename=%1

if EXIST "..\..\_temp\minimal.ino.bin" ( set binfile="..\..\_temp\minimal.ino.bin" )
if EXIST ".\_temp\minimal.ino.bin"     ( set binfile=".\_temp\minimal.ino.bin" )

echo uploading: %binfile% to device %devicename% ...
python %ESPTOOLS%\espota.py -d -r -i %devicename% -P 38288 -p 8266 -a 123 -f %binfile% 

:end
