# The plug example for the HomeDing library

This is an example that runs on ESP8266 boards that only have a small amount of Flash Memory available (1 MByte) for program and web UI storage.

This example can be used as a firmware for plugs / socket switches like the

* [Sonoff Basic](https://homeding.github.io/boards/sonoffbasic.htm)
* [Sonoff S20 device](https://homeding.github.io/boards/sonoffs20.htm)
* [Retail Plug and Socket Devices](https://homeding.github.io/boards/sockets.htm)
* and other relay based switches.

Another typical bare board for this is a [ESP8266-01 board](/boards/esp01) with 1MByte (not 512kByte) Flash Memory.

More details about this example can be found in the online documentation at

<https://homeding.github.io/examples/minimal.htm>

## Compile and upload using arduino-cli

``` txt
hd-build.bat --case plug -c plug.ino
```
