@ECHO off
rem in case someone uses `hd api` instead of `hd-api`

set _help=

:restart

if "%*"=="" ( set _help=1 )
if [%~1]==[--help] ( set _help=1 && shift /1 && goto :restart )
if [%~1]==[-h] ( set _help=1 && shift /1 && goto :restart )
if [%~1]==[/h] ( set _help=1 && shift /1 && goto :restart )
if [%~1]==[/?] ( set _help=1 && shift /1 && goto :restart )

REM == HELP screen ==

if DEFINED _help (
  echo.
  echo [1mHomeDing CLI.[0m
  echo.
  echo Use: hd ^<command^> --help 
  echo to get more help on available commands.
  echo.
  echo Commands are: api, build, backup, discover, restore, upload, webupload
  echo.
  goto :end
)

call hd-%*

:end

