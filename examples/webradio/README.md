# WebRadio Example

This example implements a web radio for ESP32 systems.

This example uses a audio element based on the ESP32 specific audio library from
<https://github.com/schreibfaul1/ESP32-audioI2S>.

This library is not available through the Arduino Library Manager
so it must be installed by downloading or git-checkout directly from github.

The esp32 board by Espressif Systems version 3.3.4 ff must be used.

See [TTGO Gallery (T14) board](https://homeding.github.io/boards/esp32/ttgogallery.htm)
for the default board that is supported by the env.json and config.json within this folder.

The AudioElement just adds a thin wrapper to configure and control the library.
The Audio processing is moved to another ESP32 FreeRTOS Task and runs in the background.

There can only be one AudioElement created per device.

## See also

* [Audio Element](https://homeding.github.io/elements/audio.htm)
* <https://homeding.github.io/boards/esp32/ttgogallery.htm>

* Other audio library: <https://github.com/pschatzmann/arduino-audio-tools>

## More Stations

Find radio streams on <https://rrradio.de/radiosender/> and <https://wiki.ubuntuusers.de/Internetradio/Stationen/>

* Das Ding <https://liveradio.swr.de/sw282p3/dasding/play.mp3>
* Unser Ding <https://liveradio.sr.de/sr/udc1/mp3/128/stream.mp3?aggregator=web>