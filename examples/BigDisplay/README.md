# The HomeDing BigDisplay Example

There are some boards available that already combine display, touch screen and processor and
additional components. This example supports these devices and provides some initial
configurations.

Boards that were used during development:

* [ESP32-8048S043C] -- ESP32-S3 with 4.3 inch 800*480 LCD touch display
* [ESP32-3248S035] -- ESP32 with 480*320 LCD touch display
* [LilyGO-T-Display-S3] -- Small ESP32-S3 boards with 170*320 LCD touch display

On the HomeDing web site you find more detailled documentation on these boards including
configuration definitions.

## Displays and Display Touch controllers

The supported displays and Display Touch controllers can be found in the HomeDing documentation
for [Display Elements](https://homeding.github.io/elements/display/index.htm).

Most of the displays are supported by the [GFX library for Arduino](https://github.com/moononournation/Arduino_GFX).


## See also

* [Display Elements](https://homeding.github.io/elements/display/index.htm)
* [Display Touch Elements](https://homeding.github.io/elements/display/touch.htm)
* [Display TouchGT911 Element](https://homeding.github.io/elements/display/touchgt911.htm)
* [Display TouchFT6336 Element](https://homeding.github.io/elements/display/touchft6336.htm)
* [Display TouchCST816 Element](https://homeding.github.io/elements/display/touchcst816.htm)
* [Capacitive vs. Resistive Touchscreens: What Are the Differences?](https://www.makeuseof.com/tag/differences-capacitive-resistive-touchscreens-si/)
* <https://wiki.makerfabs.com/Sunton_ESP32_S3_4.3_inch_800x400_IPS_with_Touch.html>


[ESP32-8048S043C]: https://homeding.github.io/boards/esp32/panel-8048S043.htm
[ESP32-3248S035]: https://homeding.github.io/boards/esp32/panel-3248S035.htm
[LilyGO-T-Display-S3]: https://homeding.github.io/boards/esp32s3\lilygo-t-display-s3.htm