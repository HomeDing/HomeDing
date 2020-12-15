# Changelog

All notable changes to this project will be documented in this file.

## [0.6.0] - 2020-12-01 

### Enhancements

* **[Display Elements](https://homeding.github.io/#page=/displays.md)**
    * **brightness** - The brightness of the display can be controlled with this action and is reported in the element state. Default is 128 of 255. 

* **[Analog Element](https://homeding.github.io/#page=/elements/analog.md)**
    * **mapIn___** and **mapOut___** properties can be used to calculate
    a meaningful range of values from the analog raw value.

    * **readtimems** - The time between capturing input values can be given in milliseconds.


* The override keyword was added to many virtual functions to express the intention and avoid type related bugs.
* Code cleanup.
* mDNS is now also broadcasting the `title` property of the device 

### Added Elements

* **[Digital Signal Element](https://homeding.github.io/#page=/elements/digitalsignal.md)**
    A new element that uses an interrupt driven approach to capture digital input signal changes.

* **[BL0937 Element](https://homeding.github.io/#page=/elements/bl0937.md)**
    A new element to support single phase energy monitoring using BL0937, HLW8012 or HJL-01 chips.

* **[MAX7219 Element]()**
    A new Element for driving a MAX7219 chip 8x8 matrix or 8x7-segment number driver using SPI.

* **[SHT20 Element]()**
    A new Element for driving the SHT20 temperature and humidity sensor using i2c bus.

## [0.5.0] - 2020-09-13 

This release focuses on stability and better developer support. New Elements is present that added required some features and fixes in the JSON parser. 


> **Important changes**
> * Specify "safemode": "false" in the device configuration when you want unsecured access to your device during setup and configuration.
> * Change "zone": "<your timezone>" in the NTPtime element using the ANSI TZ notation. Otherwise the device runs in "GMT0BST" timezone (London).
> * Support for IE11 was dropped in favor for smaller code size. Edge is supported. 
> * Build and test using the ESP8266 Community board version 2.7.4.

### Added Examples

* **[Development example](https://homeding.github.io/#page=/examples/develop.md)** - The example can be used for for experimental and incubator implementations.<br />
  In the folder you can find elements that should not be present in long-time running devices but can help wile developing.<br />
  This sketch includes all common elements and those under development or of interest.

* **[Minimal example](https://homeding.github.io/#page=/examples/minimal.md)** - The example can be compiled to get a small firmware that runs on 1MByte flash boards with reduced functionality.


### Added Elements

These Elements are added to the [Development example](https://homeding.github.io/#page=/examples/develop.md) and are in an early development state or for development purpose only. When finalized they will be moved into the core library.

* **Diag Element** - is included in the DevDing example to help analyzing the current I2C devices and memory.
* **RFSend Element** - An experimental Element for sending rf433 signals.
* **SerialCmd Element** - Use the Serial input to create actions.
* **INA219 Element** - An Element to control a INA219 power consumption chip.
* **Template Element.h** - An Element that is used as a template for implementing new elements.


### Enhancements

* **[Device Element](https://homeding.github.io/#page=/elements/device.md)** - Serval improvements.
    * **safemode** - The safemode (see <https://homeding.github.io/#page=/safemode.md>) advanced in this release but still WIP.<br/>
    The `safemode` is reported in Serial output and in the response of the `/$sysinfo` request.<br/>
    Since this release the safemode is on by default.
    * **sleeptime** - The [deep sleep mode](https://homeding.github.io/#page=/boards/deepsleep.md)
    is now supported for devices that will not be always available through the built-in web server. 
    * **cache** - The `cache` parameter can be used to switch on client side caching for the static files.
    * **sd** - The `sd` parameter can be used to switch off mDNS discovery for a device.

* **[NTPTime Element](https://homeding.github.io/#page=/elements/ntptime.md)**
    * **zone** - The NTPTime Element is now using newer the ANSI ntp time functions. See example `NTP-TZ-DST.ino`.<br/>
      The zone parameter now can include summer time adjustments.<br/>
      Changing `zone` parameter of ntptimeElement to use the POSIX TZ format.<br/> 
      Examples can be found at https://sites.google.com/a/usapiens.com/opnode/time-zones<br/>
      The system now handles refreshing the time from an NTP Server.

* **[Value Element](https://homeding.github.io/#page=/elements/value.md)**
    * **type** - The Value Element now also supports values in string format by setting "type"="string".

* **[WeatherFeed Element](https://homeding.github.io/#page=/elements/weatherfeed.md)** - finalized.
    * The WeatherFeedElement pulls weather forecast information for a specific location from 
    the internet service from <https://openweathermap> and dispatches actions with specific data items.
    * See story [Build a Weather forecast display](https://homeding.github.io/#page=/stories/story-weatherdisplay.md).

* **[DHT Element](https://homeding.github.io/#page=/elements/dht.md)**
    * **powerpin** and **powerinverse** - The DHT Element has support for a power controlling GPIO pin to reset the sensor when required.

* **[Sensor Elements](https://homeding.github.io/#page=/elements/sensors.md)**
    * **warmuptime** - the time a sensor required to be usable for first data acquiring.
    * **restart** - This parameter can be set to true when the sensor can be restarted after failure.

* **Display Elements**
    * The `clear` action on display elements (text, dot) now clears the value. 
    * The `redraw` action on display elements (text, dot, line) now resend the current value to the display adapter. 

* **Display Adapters**
    * The display Adapter implementations and their Elements have been refactored for common code and in preparation for multiple logical pages per display.
    * **show** -  the show action can be used to refresh the display.

* **CORS** - A CORS header is now set on all responses from the build-in web server to allow calling services from web pages located on other devices.

* **mDNS Service Discovery** - The HomeDing devices now publish into the local network the availability of a HomeDing device offering the typical HomeDing services. This is implemented using the mDNS-SD with the service signature `_homeding._tcp`. This is not (yet) registered as an official protocol.

* **Doku** - much source code inline doku and more documentation available on <https://homeding.github.io>.


### Fixes

* **WebUI** There are some changes in the Web UI and the build-in web updater now polls new files from <https://homeding.github.io/v02/>. Old files can now be removed by the web updater.
 
* **JSONParser** fixes, now supporting arrays in parsing.

* some **TRACE** outputs were removed from earlier implementation efforts. They are kept as commented lines or disabled macros for later debugging purpose.

* using the `new (std::nothrow)` consistently.

* The `FileServer` and `BoardServer` have minor improvements and now used the enhanced WebServer class features. 



## [0.4.1] - 2020-04-16

* JSON Parser supporting arrays.
* Avoid long procissing time without yiedl() calls on startup to get faster network connectivity. 
* Web UI updated
* internal Web UI updated
* ntp time fixed according ESP8266 board version 4.7.0
* CHANGELOG.md file added.  


## [0.4.0] - 2020-04-16

BME680 optimizations.

### Added

* BME680 sensor dashboard UI.

### Fixed

* I2C bus usage through WireUtils improved.
* use bosch bme680 instead of Adafruit library.
* external library dependency for `Adafruit Unified Sensor` and `Adafruit BME680 Library` removed.


## [0.3.1] - 2020-04-15

### Fixed

* external library dependencies.


## [0.3.0] - 2020-04-14

Initial release after beta phase.


## Notes

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html) as used for the Arduino libraries.

