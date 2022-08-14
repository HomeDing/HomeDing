@echo off
echo.
echo [1mHomeDing Firmware OTA upload utility.[0m
echo.

set _binfolder=.\bin

set ESPTOOLS=
if EXIST "%LOCALAPPDATA%\Arduino15\packages\esp8266\hardware\esp8266\3.0.2\tools" ( set ESPTOOLS="%LOCALAPPDATA%\Arduino15\packages\esp8266\hardware\esp8266\3.0.2\tools")
if EXIST "%USERPROFILE%\Projects\Arduino\Sketches\Hardware\esp8266com\esp8266\tools" ( set ESPTOOLS="%USERPROFILE%\Projects\Arduino\Sketches\Hardware\esp8266com\esp8266\tools")

set _help=
if [%1]==[--help] ( set _help=1 & shift /1 )
if [%1]==[-h] ( set _help=1 & shift /1 )
if [%1]==[] ( set _help=1 )

if DEFINED _help (
  echo This is a helpful wrapper on Windows for the standard upload tool "espota.py" that
  echo searches for the latest build to upload to the specified device.
  echo Usage: hd-upload.bat [parameters] ^<devicename^> [firmware]
  echo.  -h, --help     Get a brief help on using this tool
  echo.  -p pass        use `pass` as upload password
  echo.
  echo The bin file is searched in the [1mbuild[0m subdirectory 
  echo When specifying a firmware name the file is taken from the [1mbin[0m directory. 
  echo.
)

set _pass=123
if [%1]==[-p] ( set _pass=%2 & shift /1 & shift /1 )

echo %_pass%

set devicename=%1
set firmware=%2
set binfile=

if NOT DEFINED devicename (
  echo [31m--missing ^<devicename^> as parameter 1--[30m
  goto :end
)

rem == find / verify firmware filename == 

if NOT DEFINED firmware (
  @REM find last built bin file
  FOR %%F IN (.\build\*.bin) DO (
    @REM echo [37m  %%F[30m
    set binfile=%%F%
  )

) ELSE (
  FOR %%F IN (%_binfolder%\%firmware%*.bin) DO (
    echo [37m  %%F[30m
    set binfile=%%F%
  )
)


rem == verify parameters == 

if NOT defined binfile (
  echo [31m--no firmware file found--[30m
  goto :end
)

ping -w 1000 -n 1 %devicename% >nul:
if ERRORLEVEL 1 (
  echo [31mDevice not found[30m
  goto :end
)


rem == execute == 

echo [1mUploading...[0m
echo.  Device name = %devicename%
echo.  Firmware    = %binfile%
echo.  Password    = %_pass%
echo.

python %ESPTOOLS%\espota.py -d -r -i %devicename% -P 38288 -p 8266 -a %_pass% -f %binfile% 
echo.

echo [1mdone.[0m

:end

set binfile=
set _pass=
set _help=
echo.
