@echo off

echo.
echo [1mBuild for Plug.[0m
echo.

set _hd-fqbn=esp8266:esp8266:generic
set _hd-properties="xtal=80,vt=flash,exception=disabled,stacksmash=disabled,ssl=basic,mmu=3232,non32xfer=fast,ResetMethod=nodemcu,CrystalFreq=26,FlashFreq=40,FlashMode=dout,eesz=1M128,led=2,sdk=nonosdk_190703,ip=lm2f,dbg=Disabled,lvl=None____,wipe=none,baud=921600"

