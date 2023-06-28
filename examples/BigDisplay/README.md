# The HomeDing BigDisplay Example

This example exists for supporting devices with a built-in
display, touch screen, processor and additional components like sd card.

The combination of elements enabled in this example therefore include
Elements for Resistive and Capacitive touch controllers.

* GT911 (5 point Capacitive)
<!--
* <https://github.com/PaulStoffregen/XPT2046_Touchscreen>
-->


It is verified with these boards

**ESP32-3248S035** -- ESP32 with 480*320 LCD touch display.

* Board: ESP32 Dev Module
* PSRAM: disabled
* CPU: 240 MHz
* Flash Mode: DIO
* Flash: 4MB


IO34 = LCD
SD Card via SPI
RGB LED
Touch (I2C)
TFT (SPI)

``` JSON
{
  "device": {
    "0": {
      "name": "panel480",
      "title": "Panel480",
      "description": "Panel with 480 px",
      "loglevel": "2",
      "xbutton": "D3",
      "xled": "4",
      "logfile": 1,
      "safemode": "false",
      "homepage": "/board.htm",
      "cache": "max-age=600",
      "i2c-SDA": "33",
      "i2c-SCL": "32"
    }
  },
  "ota": {
    "0": {
      "port": 8266,
      "passwd": "123"
    }
  },
  "ntptime": {
    "on": {
      "zone": "CET-1CEST,M3.5.0,M10.5.0/3"
    }
  },
  "diag": {
    "0": {}
  },
  "light": {
    "l": {
      "pin": "4,16,17",
      "mode": "pwm",
      "invert": "true",
      "enable": "true"
    }
  },
  "color": {
    "l": {
      "title": "RGB",
      "loglevel": 2,
      "config": "RGB",
      "mode": "fix",
      "duration": "4s",
      "value": "x004466aa",
      "brightness": "20",
      "connect": [ "light/l" ]
    }
  },
  "DisplayST7796": {
    "0": {
      "height": "480",
      "width": "320",
      "color": "x000000",
      "background": "xccccff",
      "lightpin": 27
    }
  },
  "DisplayTouchGT911": {
    "0": {
"address": "0x5D"}
  }
}
```

**ESP32-8048S043** **C** -- ESP32S3 with 4.3 inch 800*480 LCD and capacitive touch display.

* Display: ST7262: 4.3 inch, 800 * 480 px, 16 bit color, RGBPanel mode
* Touch Sensor: GT911
* ESP32-S3 Processor
* 16M Flash Memory
* 8M PSRAM
* SD Card

See board documentation at <https://homeding.github.io/boards/esp32/panel-8048S043.htm>

---

[ESP32-8048S043](http://www.jczn1688.com/zlxz?spm=a2g0o.detail.1000023.1.3ef123b9JUyyZm)

* <https://forum.arduino.cc/t/esp32-4827s043-does-not-work-with-the-sd-card/1056652>
* Support files <http://www.jczn1688.com/zlxz> Download passwort: jczn1688
* Display driver <https://github.com/lovyan03/LovyanGFX/blob/master/src/lgfx_user/LGFX_ESP32S3_RGB_ESP32-8048S043.h>
* Display driver


## Topics

Implement a display adapter apart from the core library.
(here RGB Panel 800*480 px with DMA based data transfer)

Plug in DisplayElement

Plug in touch panel element

<!-- <https://wiki.makerfabs.com/Sunton_ESP32_S3_4.3_inch_800x400_IPS_with_Touch.html> -->

<!-- https://www.makerfabs.com/sunton-esp32-s3-4-3-inch-ips-with-touch.html -->

## See also

* [Display Elements](https://homeding.github.io/elements/display/touch.htm)
* [Display TouchGT911 Element](https://homeding.github.io/elements/display/touchgt911.htm)
* [Capacitive vs. Resistive Touchscreens: What Are the Differences?](https://www.makeuseof.com/tag/differences-capacitive-resistive-touchscreens-si/)
* <https://wiki.makerfabs.com/Sunton_ESP32_S3_4.3_inch_800x400_IPS_with_Touch.html>
*

---

``` txt

waiting for IDF 5.1 in espressif/arduino ...

## Adafruit_GFX restrictions and rework requests

Canvas classes cannot be sub-classed for implementing specific displays:

* The buffer allocation is inside the provate implementation and cannot be changed.
Some displays require display memory to be allocated in a specific way e. g. when display is using DMA for data transfer.

* Colors are fixed to maximum 16 bit (physical) within all color parameters
  but displays supporting rgb888 (24 bit) are already available.

* Graphical "objects" like buttons should by refactored to be additional objects.
  As they may overlap or update on their own a layer upon the GFX is required to redraw defined objects when required.

* Some implementations treat "textsize" as a multiple of the builtin font size. A "registered/available" fonts manager is missing.

* uint16_t color - to color type definitions
* may include a color scheme for objects.

* buffer memory allocation and handling is 
```
