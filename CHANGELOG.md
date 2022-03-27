# Changelog

All notable changes to this project will be documented in this file.

## [0.8.2] - 2022-03-28

The released version 0.7.2 was stable enough to do many implementations so many changes
where not published in a release.

### Breaking Changes

With the 0.8.x version the SPIFFS filesystem is not supported any more
resulting a a reformatted flash disk space when updating.

* **LittleFS** -
  LitteFS is now the default filesystem.
  
  As the SPIFFS filesystem is deprecated the LittleFS filesystem is supported from now on.

  When updating the configuration files and any changed files in the filesystem
  should be downloaded because they will be deleted by the new firmware like on a fresh restart.
  The WiFi configuration is persisted outside the filesystem.

  This version still supports both implementations, see filesystem initializing in main sketch.

### Changes / Enhancements

* The redirection handler is moved into board server resulting in reduced sketch files.
* The BuiltinHandler class is handling all server request with static ressources.
* The built-in services implemented by the boardserver can now be reached using the
  `/api/` route without the `$` character. E.g. <http://homeding/api/sysinfo>.
* The API calls can be made on the /api/ url path in addition of using the $... paths
  that will be deprecated. /$... paths will stay for builtin files.
* /$update.htm now served with version parameter based on flash disk size.
* The display adapters and elements have beedn rewritten to use the Adafruit GFX library.

* MUCH documentation at [github.io](https://homeding.github.io).

A lot new elements and examples:

* **[TM1637 Element](https://homeding.github.io/#page=/elements/tm1637.md)** -
    new element supporting 4-digit time displays with the tm1637 chip.

* **[State Element](https://homeding.github.io/#page=/elements/state.md)** -

* **[RTCState Element](https://homeding.github.io/#page=/elements/rtcstate.md)** -

* **[ADD Element](https://homeding.github.io/#page=/elements/add.md)** -
    a new element adding multiple input values.

* **[AND Element](https://homeding.github.io/#page=/elements/and.md)** -
    a new element combining multiple input values using the logical AND function.

* **[OR Element](https://homeding.github.io/#page=/elements/or.md)** -
    a new element combining multiple input values using the logical OR function.

* **[Probe Example](https://homeding.github.io/#page=/examples/probe.md)** -
    a new example providing a sketch and extra Elements to build a power consumption monitoring device using INA219 and ina226 chips.

* **[Probe Example](https://homeding.github.io/#page=/examples/probe.md)** - A new example to implement current sensor with long time logging using INA219 or INA226 sensor chips.

* saving state of elements to survive a boot / but no power down.
* arduino-lint github action

* startup without files in filesystem will cause ota to be started to enable update over the wire.

* The **[DHT Element](https://homeding.github.io/#page=/elements/dht.md)** supporting the DHT11, DHT22 temperature and humidity sensor is switched to another library.

* The mDNS Protocol is now also used to publish the embedded web server.

## [0.7.0] - 2021-06-15

The version 0.7.0 was created to support the version 3.x of ESP8266 community board support.

The ESP32 boards are supported with some exceptions (e.g. MY9291Element)
and is still more beta than ESP8266 board support.

### Changes / Enhancements

* RFBridge example

  This example is "work in progress" (WIP).

  It has created to separate the bridge supporting RFCodes Element out of the standard set of elements
  to avoid that the RFCodes library is required all the time.

  There will be a specific story in the HomeDing documentation at <https://homeding.github.io/#page=/stories.md>
  on this example when work is done.

* [BME680 Element](https://homeding.github.io/#page=/elements/bme680.md) Bosch library updated to version 1.6.1480

* [State implementation for Elements]()

  new, to be documented.

* [Color Element]

  new, to be documented.

* [Timer Element](https://homeding.github.io/#page=/elements/timer.md)

  **mode** - The Timer can be disabled by setting mode to "ON" or "OFF". To re-enable the timer set mode to "TIMER".

  **reset** - The automatically (re-)start the timer set "restart" to true. The older "type":"loop" is removed.

  The UI for the element has been enhanced.

* **Display Adapters** and **Display Elements**

  **onPage** - These actions can be defined on the display elements and are triggered every time a new page is displayed.

  **page** - This property can be set on the display elements to display a specific page like `displaysh1106/0?page=2`

  **addpage** - This property can be set on the display elements to display the next or previous page `displaysh1106/0?addpage=-1`

  The Web UI of the display elements now has 2 arrow buttons to flip through the used / defined pages.

* **DisplayOutput Elements**

  **page** - This property can be set on display the text, line or other visuals on a specific page.

## [0.6.0] - 2021-01-03

### Enhancements

* **[Display Elements](https://homeding.github.io/#page=/displays.md)**

  **brightness** - The brightness of the display can be controlled with this action and is reported in the element state. Default is 128 of 255.

* **[Analog Element](https://homeding.github.io/#page=/elements/analog.md)**

  **mapIn___** and **mapOut___** properties can be used to calculate
  a meaningful range of values from the analog raw value.

  **readTimeMS** - The time between capturing input values can be given in milliseconds.

  The **override** keyword was added to many virtual functions to express the intention and avoid type related bugs.

  Code cleanup.

  mDNS is now also broadcasting the `title` property of the device

### Added Elements

* **[Digital Signal Element](https://homeding.github.io/#page=/elements/digitalsignal.md)**
    A new element that uses an interrupt driven approach to capture digital input signal changes.

* **[BL0937 Element](https://homeding.github.io/#page=/elements/bl0937.md)**
    A new element to support single phase energy monitoring using BL0937, HLW8012 or HJL-01 chips.

* **[MAX7219 Element](https://homeding.github.io/#page=/elements/max7219.md)**
    A new Element for driving a MAX7219 chip 8x8 matrix or 8x7-segment number driver using SPI.

* **[SHT20 Element](https://homeding.github.io/#page=/elements/sht20.md)**
    A new Element for driving the SHT20 temperature and humidity sensor using i2c bus.

* **[Reference Element](https://homeding.github.io/#page=/elements/reference.md)**
    A new Element for calculating a boolean from comparing a value against a reference value.

## [0.5.0] - 2020-09-13

This release focuses on stability and better developer support. New Elements is present that added required some features and fixes in the JSON parser.

> **Important changes**
>
> * Specify "safeMode": "false" in the device configuration when you want unsecured access to your device during setup and configuration.
> * Change "zone": "<your timezone>" in the ntpTime element using the ANSI TZ notation. Otherwise the device runs in "GMT0BST" timezone (London).
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
  * **safeMode** - The safeMode (see <https://homeding.github.io/#page=/safemode.md>) advanced in this release but still WIP.<br/>
    The `safeMode` is reported in Serial output and in the response of the `/$sysinfo` request.<br/>
    Since this release the safeMode is on by default.
  * **sleepTime** - The [deep sleep mode](https://homeding.github.io/#page=/boards/deepsleep.md)
    is now supported for devices that will not be always available through the built-in web server.
  * **cache** - The `cache` parameter can be used to switch on client side caching for the static files.
  * **sd** - The `sd` parameter can be used to switch off mDNS discovery for a device.

* **[NTPTime Element](https://homeding.github.io/#page=/elements/ntptime.md)**
  * **zone** - The NTPTime Element is now using newer the ANSI ntp time functions. See example `NTP-TZ-DST.ino`.<br/>
      The zone parameter now can include summer time adjustments.<br/>
      Changing `zone` parameter of ntpTimeElement to use the POSIX TZ format.<br/>
      Examples can be found at <https://sites.google.com/a/usapiens.com/opnode/time-zones><br/>
      The system now handles refreshing the time from an NTP Server.

* **[Value Element](https://homeding.github.io/#page=/elements/value.md)**
  * **type** - The Value Element now also supports values in string format by setting "type"="string".

* **[WeatherFeed Element](https://homeding.github.io/#page=/elements/weatherfeed.md)** - finalized.
  * The WeatherFeedElement pulls weather forecast information for a specific location from
    the internet service from <https://openweathermap> and dispatches actions with specific data items.
  * See story [Build a Weather forecast display](https://homeding.github.io/#page=/stories/story-weatherdisplay.md).

* **[DHT Element](https://homeding.github.io/#page=/elements/dht.md)**
  * **powerPin** and **powerInverse** - The DHT Element has support for a power controlling GPIO pin to reset the sensor when required.

* **[Sensor Elements](https://homeding.github.io/#page=/elements/sensors.md)**
  * **warmupTime** - the time a sensor required to be usable for first data acquiring.
  * **restart** - This parameter can be set to true when the sensor can be restarted after failure.

* **Display Elements**
  * The `clear` action on display elements (text, dot) now clears the value.
  * The `redraw` action on display elements (text, dot, line) now resend the current value to the display adapter.

* **Display Adapters**
  * The display Adapter implementations and their Elements have been refactored for common code and in preparation for multiple logical pages per display.
  * **show** -  the show action can be used to refresh the display.

* **CORS** - A CORS header is now set on all responses from the build-in web server to allow calling services from web pages located on other devices.

* **mDNS Service Discovery** - The HomeDing devices now publish into the local network the availability of a HomeDing device offering the typical HomeDing services. This is implemented using the mDNS-SD with the service signature `_homeding._tcp`. This is not (yet) registered as an official protocol.

* **Documentation** - much source code inline annotations and more documentation available on <https://homeding.github.io>.

### Fixes

* **WebUI** There are some changes in the Web UI and the build-in web updater now polls new files from <https://homeding.github.io/v02/>. Old files can now be removed by the web updater.

* **JSONParser** fixes, now supporting arrays in parsing.

* some **TRACE** outputs were removed from earlier implementation efforts. They are kept as commented lines or disabled macros for later debugging purpose.

* using the `new (std::nothrow)` consistently.

* The `FileServer` and `BoardServer` have minor improvements and now used the enhanced WebServer class features.

## [0.4.1] - 2020-04-16

* JSON Parser supporting arrays.
* Avoid long processing time without yield()/delay(1) calls on startup to get faster network connectivity.
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
