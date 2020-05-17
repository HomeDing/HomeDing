# Changelog

All notable changes to this project will be documented in this file.

## [Unreleased]

### Added

* JSON Parser supporting arrays.
* Avoid long procissing time without yiedl() calls on startup to get faster network connectivity. 
* Web UI updated
* ntp time fixed according ESP8266 board version 4.7.0
* CHANGELOG.md file added.  


## [0.4.0] - 2020-04-16

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

