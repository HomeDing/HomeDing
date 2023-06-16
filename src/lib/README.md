# Folder for included libraries

## GT911

The GT911 is a 5-Point Capacitive Touch Sensors for 7" to 8" panels using the I2c bus for
communication. It can deliver Input results every 40 / 10 msec.

There are multiple implementations - even some for arduino but they are all in an early stage or
not maintained any more.

* <https://github.com/arduino-libraries/Arduino_GigaDisplayTouch> This library is in an early
  stage and implemented for Arduino boards based on mbed operating system only. I like the
  cleaness of the interface. Rotation support is missing as well.
* <https://github.com/TAMCTec/gt911-arduino> This library is almost working but has some buffer
  overflows when communication is not correct (found more than 5 points) and needs software reset.
* <https://github.com/u4mzu4/Arduino_GT911_Library>

The gt911.h / gt911.cpp is my own assembled version working for ESP32.

### See also

* <https://github.com/goodix> reference implementation for Android
* <https://www.goodix.com/en/product/touch/touch_screen_controller>
* <https://github.com/lvgl/lvgl_esp32_drivers/blob/master/lvgl_touch/gt911.h>

