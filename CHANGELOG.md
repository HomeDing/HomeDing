# Changelog

All notable changes to this project will be documented in this file.

## [Unreleased]

RF Bridge example

### Added

* DevDing example folder for experimental and incubator implementations.
* Reporting "savemode" in Serial output
* Reporting "savemode" in /$sysinfo request

### Changed Elements

* **NTPTime** - using newer ANSI ntp time functions. See example `NTP-TZ-DST.ino`.<br/>
  The zone parameter now can include summer time adjustments.<br/>
  Changing `zone` parameter of ntptimeElement to use POSIX TZ conformant string.<br/> 
  Examples can be found at https://sites.google.com/a/usapiens.com/opnode/time-zones


### New Elements

* **CORS** The CORS header is now set on all responses from the build-in web server to allow calling services from web pages located on other devices.

* **mDNS Service Discovery** the HomeDing devices now publish into the local network the availability of a HomeDing device offering the typical HomeDing services. This is implemented using the mDNS-SD with the service signature `_homeding._tcp`. This is not (yet) registered as an official protocol.

* **WeatherFeed** Element got finalized.<br />
  See docu. at https://homeding.github.io/#page=/elements/weatherfeed.md.

* **Diag** Element is included in the DevDing Example to help analyzing the current I2C devices and memory.

### Fixed

* JSONParser fixes.

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

