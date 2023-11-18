# Changelog

All notable changes to this project will be documented in this file.

## [0.9.x] - unreleased

### Display and Touch Driver updates

There is support for displays in the HomeDing Library since a long time. With the ESP32 variants
come some new opportunities especially for larger and more colorful display like 480*320 px including touch controllers.
This version has some major updates to support these devices.

* Some Display Adapters are now implemented using the [Arduino_GFX
  Library](https://github.com/moononournation/Arduino_GFX) that supports a long list of
  interfaces called [Data bus](https://github.com/moononournation/Arduino_GFX/wiki/Data-Bus-Class)
  and displays [Display Class](https://github.com/moononournation/Arduino_GFX/wiki/Display-Class).
  Also many boards including displays are explained how to use.

  This library is derived from the AdaFruit library and was extended to support more displays
  than sold by AdaFruit.

  A must recommend displays from AdaFruit as they take care about the quality of their items
  (that is not always true when importing from china or buying from eBay).

* The [Display Touch GT911 Element](https://homeding.github.io/elements/display/touchgt911.htm)
  is the implementation of a touch controller that comes with displays.

* The [Display Touch CST816 Element](https://homeding.github.io/elements/display/touchcst816.htm)
  is the implementation of a touch controller that comes with displays.

* The [Display Touch FT6336 Element](https://homeding.github.io/elements/display/touchft6336.htm)
  is the implementation for the touch controller that comes with displays using the ft6336 chip.

* The [Display Touch Element](https://homeding.github.io/elements/display/touch.htm) is the base
  implementation for touch controllers that cannot be instantiated as an element. It handles
  click events and shares them with the available
  [Display Button Elements](https://homeding.github.io/elements/display/button.htm). Now you can add visual buttons on the display to start actions by click.

* The [Display Button Element](https://homeding.github.io/elements/display/button.htm) draws a
  button at the configured position where click events should be captured.

* The BigDisplay example is made for a display with integrated touch panel.
<!-- -> story -->

* Rewriting of the startup sequence to speed up startup and remove of sysLED and sysButton. See
  [The Startup Sequence](https://homeding.github.io/dev/startup.md) and [Persisting Current
  State of Elements](https://homeding.github.io/elements/state.md).

* The **Diag Element** included in the DevDing example was extended with the endpoints:

  * /diag (build info, state, i2c devices)
  * /profile (profiling the loop() times of elements)
  * /chipinfo (about the chip in use)

  Some related code was removed from the board.cpp implementation.

* Profiling the loop() times can be enabled by including the <hdProfile.h> in <homeding.h>
  manually, recompile and upload the extended sketch.

  This will result in recoding the time consumed by the loop() function of elements to identify
  non-cooperative elements.

  The [Diag Element](https://homeding.github.io/elements/diag.htm) provides a simple web page
  showing the current recorded times using `http://devicename/profile`.


### Minimal Examples

The minimal example was split into **plug** and **bulb**.

These [Minimal Examples](https://github.com/HomeDing/HomeDing/tree/develop/examples/minimal)
are implemented to support ESP8266 based devices with 1MB Flash like switches and lights.
They contain a reduced set of elements for the specific use-case and a small footprint Web-UI.
These work well with small self-made and and off-the-shell devices.

The micro example is especially for ESP8266 based boards with 1 MByte Flash only
to support a 2-step ´firmware update.
It can be flashed on a devices even when memory is low for a regular update.
After the micro example is flashed a bigger firmware can be flashed.

Some examples also have a \<sketch\>.ino.globals.h file to define a Marco "HD_MINIMAL".
This is causing smaller bin files by removing some rarely needed functionality.


## [0.9.7] - 2023-05-01 unreleased

* updated Web UI files from Web project <https://github.com/HomeDing/WebFiles>.

* [State Elements](https://homeding.github.io/elements/state.htm) documentation added.

* [SDMMC Element](https://homeding.github.io/elements/sdmmc.htm) for ESP32 to add files from sd card to web server
  using ESP32 specific sdmmc interface.
  .
* [SD Element](https://homeding.github.io/elements/sd.htm) add files from sd card to web server using the SPI interface.

* ESP32-S3 Processor Support
  
  **Important** -- The SSDP discovery libraray for ESP32 must be installed manually
  as it is not listed in the Arduino Library Manager.
  It can be found at <https://github.com/luc-github/ESP32SSDP>
  (download zip file and extract into the libraries folder or `git clone https://github.com/luc-github/ESP32SSDP`)

* The Examples on [How to create and extend a new Element](https://homeding.github.io/steps/newelement.htm)
  have been moved into the [tutorial example](https://homeding.github.io/examples/tutorial.htm) folder.

  Documentation has been updated, see <https://homeding.github.io>.

### Changes / Enhancements

* ESP32-C3 Processor Support

  The ESP32-C3 is positioned as a cost-effective RISC-V MCU
  with Wi-Fi and Bluetooth 5 (LE) connectivity for secure IoT applications.
  It is often seen as a replacement option for the ESP8266.

  The ESP32-C3 offers less IO pins that the ESP32 or ESP32-S3 chips and comes in a smaller 32 pin QFN32 package (5×5 mm).

  See [ESP32-C3 Boards](https://homeding.github.io/boards/esp32/esp32c3.htm) documentation added.

* [WebRadio example](https://github.com/HomeDing/HomeDing/tree/develop/examples/webradio)
  
  This example implements a audio streaming device for ESP32 based systems.

* The [Display ST7789 Element](https://homeding.github.io/displays/st7789.htm)
  supports color LCD displays using the ST7789 driver chip.

* The [Display ST7735 Element](https://homeding.github.io/displays/st7735.htm)
  supports color LCD displays using the ST7735 driver chip.

* The Color and Light Elements have been refactored.
  Advanced control over light elements
  should be done by the ColorElement.

* Completed ESP32 Support

  The ESP32 processor is now fully supported.
  Missing features have been implemented.
  
  Please note that caused by different processor functionality
  not all features and elements are supported for all processors.

* The /$board endpoints have been removed. Use the /api/** endpoints instead.

* **micro** example

  This is not a real example but is used to flash small devices
  with firmware for specific conditions.
  See [micro example readme](/examples/micro/README.md)


## [0.9.0] - 2022-08-22

### Changes / Enhancements

* There are many enhancements and bug fixing in this release.

* The /api endpoints are now standard all $board services are deprecated and will be removed in version 1.0

* **[MQTT Element](https://homeding.github.io/elements/mqtt.htm)** --  
  supports sending values to MQTT servers based on actions.

* **[DallasElement](https://homeding.github.io/elements/dallas.htm)** --  
  is an enhanced version of the former Ds18B20Element  
  now supporting different sensors using the OneWire protocol.

* **[BH1750 Element](https://homeding.github.io/elements/bh1750.htm)** --  
  is supporting the BH1750 Light sensor.  
  See now supporting different sensors using the OneWire protocol.

* **[BH1750 Element](https://homeding.github.io/elements/am2320.htm)** --
The AM2320Element allows retrieving temperature and humidity
values from AM2320 sensors and creates actions when new values are available.

* **[AHT20 Element](https://homeding.github.io/elements/aht20.htm)** --
The AHT20Element allows retrieving temperature and humidity
values from from the AHT20 sensors from aosong and creates actions when new values are available.

* **[SDC4x Element]**
A Element to create actions based on sensor values from a SCD4X Temperature and Humidity sensor.

* **[Touch Element](https://homeding.github.io/elements/touch.htm)** --
The TouchElement enables creating Actions by using touch input on the ESP32.

* **[DisplayMAX7219 Element] to use more than 4 modules (WIP).

* **SensorElement** is enhanced to support reporting the state and emitting actions for the sensor values
  to simplify sensor implementations and save overall memory.

* **[CalcElement](https://homeding.github.io/elements/calc.htm)** is used as a base class
for some elements with multiple input an calculated output.

* There are **[CLI commands](https://homeding.github.io/dev/cli.htm)** available for updating and restoring device configurations
  and uploading pre-build firmware.

* much fixing, testing, documentation and cleanup  

* much more support for ESP32.  

## [0.8.2] - 2022-03-28

The released version 0.7.2 was stable enough to do many implementations so many changes
where not published in a release.

This release includes support for ESP32 based implementations in addition to ESP8266.

### Breaking Changes

With the 0.8.x version the SPIFFS filesystem is not supported any more
resulting a a reformatted flash disk space when updating.

* **LittleFS** -
  LittleFS is now the default filesystem.
  
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
* The display adapters and elements have been rewritten to use the Adafruit GFX library.

* MUCH documentation at [github.io](https://homeding.github.io).

A lot new elements and examples:

* **[TM1637 Element](https://homeding.github.io/elements/tm1637.htm)** -
    new element supporting 4-digit time displays with the tm1637 chip.

* **[State Element](https://homeding.github.io/elements/state.htm)** -

* **[RTCState Element](https://homeding.github.io/elements/rtcstate.htm)** -

* **[ADD Element](https://homeding.github.io/elements/add.htm)** -
    a new element adding multiple input values.

* **[AND Element](https://homeding.github.io/elements/and.htm)** -
    a new element combining multiple input values using the logical AND function.

* **[OR Element](https://homeding.github.io/elements/or.htm)** -
    a new element combining multiple input values using the logical OR function.

* **[MAP Element](https://homeding.github.io/elements/map.htm)** -
* **[Scene Element](https://homeding.github.io/elements/scene.htm)** -


* **[Probe Example](https://homeding.github.io/examples/probe.htm)** -
  A new example providing a sketch and extra Elements to build a power consumption monitoring device using INA219 and ina226 chips.

* **[RFBridge Example](https://homeding.github.io/examples/rfbridge.htm)** -
  A new example to implement a RF433 bridge device sending and receiving 433MHz signals.

* saving state of elements to survive a boot / but no power down.
* arduino-lint github action

* startup without files in filesystem will cause ota to be started to enable update over the wire.

* The **[DHT Element](https://homeding.github.io/elements/dht.htm)** supporting the DHT11, DHT22 temperature and humidity sensor is switched to another library.

* The mDNS Protocol is now also used to publish the embedded web server.

* The startup sequence and WiFi Manager implementation was improved.

## [0.7.0] - 2021-06-15

The version 0.7.0 was created to support the version 3.x of ESP8266 community board support.

The ESP32 boards are supported with some exceptions (e.g. MY9291Element)
and is still more beta than ESP8266 board support.

### Changes / Enhancements

* RFBridge example

  This example is "work in progress" (WIP).

  It has created to separate the bridge supporting RFCodes Element out of the standard set of elements
  to avoid that the RFCodes library is required all the time.

  There will be a specific story in the HomeDing documentation at <https://homeding.github.io/stories.htm>
  on this example when work is done.

* [BME680 Element](https://homeding.github.io/elements/bme680.htm) Bosch library updated to version 1.6.1480

* [State Elements](https://homeding.github.io/elements/state.htm)

  The [State Elements](https://homeding.github.io/elements/state.htm) allow saving current element values that will be restored after a reset or deep sleep without power loss.

* [Color Element]

  The [Color Element](https://homeding.github.io/elements/color.htm) allows
  controlling a value for a light emitting element using the
  color values in the xrrggbb or xwwrrggbb format and brightness 0..100.
  A special Web UI is available.

* [Timer Element](https://homeding.github.io/elements/timer.htm)

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

* **[Display Elements](https://homeding.github.io/displays.htm)**

  **brightness** - The brightness of the display can be controlled with this action and is reported in the element state. Default is 128 of 255.

* **[Analog Element](https://homeding.github.io/elements/analog.htm)**

  **mapIn___** and **mapOut___** properties can be used to calculate
  a meaningful range of values from the analog raw value.

  **readTimeMS** - The time between capturing input values can be given in milliseconds.

  The **override** keyword was added to many virtual functions to express the intention and avoid type related bugs.

  Code cleanup.

  mDNS is now also broadcasting the `title` property of the device

### Added Elements

* **[Digital Signal Element](https://homeding.github.io/elements/digitalsignal.htm)**
    A new element that uses an interrupt driven approach to capture digital input signal changes.

* **[BL0937 Element](https://homeding.github.io/elements/bl0937.htm)**
    A new element to support single phase energy monitoring using BL0937, HLW8012 or HJL-01 chips.

* **[MAX7219 Element](https://homeding.github.io/elements/max7219.htm)**
    A new Element for driving a MAX7219 chip 8x8 matrix or 8x7-segment number driver using SPI.

* **[SHT20 Element](https://homeding.github.io/elements/sht20.htm)**
    A new Element for driving the SHT20 temperature and humidity sensor using i2c bus.

* **[Reference Element](https://homeding.github.io/elements/reference.htm)**
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

* **[Development example](https://homeding.github.io/examples/develop.htm)** - The example can be used for for experimental and incubator implementations.<br />
  In the folder you can find elements that should not be present in long-time running devices but can help wile developing.<br />
  This sketch includes all common elements and those under development or of interest.

* **[Minimal example](https://homeding.github.io/examples/minimal.htm)** - The example can be compiled to get a small firmware that runs on 1MByte flash boards with reduced functionality.

### Added Elements

These Elements are added to the [Development example](https://homeding.github.io/examples/develop.htm) and are in an early development state or for development purpose only. When finalized they will be moved into the core library.

* **Diag Element** - is included in the DevDing example to help analyzing the current I2C devices and memory.
* **RFSend Element** - An experimental Element for sending rf433 signals.
* **SerialCmd Element** - Use the Serial input to create actions.
* **INA219 Element** - An Element to control a INA219 power consumption chip.
* **Template Element.h** - An Element that is used as a template for implementing new elements.

### Enhancements

* **[Device Element](https://homeding.github.io/elements/device.htm)** - Serval improvements.
  * **safeMode** - The safeMode (see <https://homeding.github.io/safemode.htm>) advanced in this release but still WIP.<br/>
    The `safeMode` is reported in Serial output and in the response of the `/$sysinfo` request.<br/>
    Since this release the safeMode is on by default.
  * **sleepTime** - The [deep sleep mode](https://homeding.github.io/boards/deepsleep.htm)
    is now supported for devices that will not be always available through the built-in web server.
  * **cache** - The `cache` parameter can be used to switch on client side caching for the static files.
  * **sd** - The `sd` parameter can be used to switch off mDNS discovery for a device.

* **[NTPTime Element](https://homeding.github.io/elements/ntptime.htm)**
  * **zone** - The NTPTime Element is now using newer the ANSI ntp time functions. See example `NTP-TZ-DST.ino`.<br/>
      The zone parameter now can include summer time adjustments.<br/>
      Changing `zone` parameter of ntpTimeElement to use the POSIX TZ format.<br/>
      Examples can be found at <https://sites.google.com/a/usapiens.com/opnode/time-zones><br/>
      The system now handles refreshing the time from an NTP Server.

* **[Value Element](https://homeding.github.io/elements/value.htm)**
  * **type** - The Value Element now also supports values in string format by setting "type"="string".

* **[WeatherFeed Element](https://homeding.github.io/elements/weatherfeed.htm)** - finalized.
  * The WeatherFeedElement pulls weather forecast information for a specific location from
    the internet service from <https://openweathermap> and dispatches actions with specific data items.
  * See story [Build a Weather forecast display](https://homeding.github.io/stories/story-weatherdisplay.htm).

* **[DHT Element](https://homeding.github.io/elements/dht.htm)**
  * **powerPin** and **powerInvert** - The DHT Element has support for a power controlling GPIO pin to reset the sensor when required.

* **[Sensor Elements](https://homeding.github.io/elements/sensors.htm)**
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
