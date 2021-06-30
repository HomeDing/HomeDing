# The HomeDing Library

[![Arduino Library Checks](https://github.com/HomeDing/HomeDing/actions/workflows/checks.yml/badge.svg)](https://github.com/HomeDing/HomeDing/actions/workflows/checks.yml)
[![arduino-library-badge](https://www.ardu-badge.com/badge/HomeDing.svg?)](https://www.ardu-badge.com/HomeDing)
[![GitLicense](https://gitlicense.com/badge/HomeDing/HomeDing)](https://gitlicense.com/license/HomeDing/HomeDing)

The HomeDing library for Arduino is a low code solution and library to build small IoT devices
without the need for cloud services or a local central device.
It is a complete ecosystem of IoT specific elements that can be activated and configured on demand without compiling a new firmware.
It is extensible by adding new specific elements.

The devices run stand-alone only connected to the local WiFi and provide
information and control of the device using a built-in web server.

Documentation can be found in at [HomeDing documentation site](https://homeding.github.io)

This Library is published using a [BSD 3-clause license](./LICENSE).


### support of many sensors, displays and other IO

The Homeding library has implemented many [Elements](https://homeding.github.io/#page=/elements.md) for
simple Input and Output of signals,
[sensors](https://homeding.github.io/#page=/sensors/sensors.md),
[displays](https://homeding.github.io/#page=/displays.md), lightning, logical processing, time support and many others.

The embedded Web Server enables configuration and control of the device.

It supports:

* a device specific UI that can be accessed by browsers and mobile devices
* web services using the REST format to control the device
* web services using the REST format to get the status of the device
* retrieving actions from other devices
* a small IDE (Integrated Development Environment) that allows changing the configuration

More documentation can be found in the  [HomeDing documentation site](https://homeding.github.io)


## BSD License

The HomeDing library is open-source and uses one of the most permissive licenses so you can use it for any project:
* Private use
* Commercial use
* You can modify
* You can distribute

If you like to contribute by additions or bug fixes you are welcome to open issues or pull requests on github.

If you plan to use it in a commercial product you are welcome to contact me regarding working and planning the future.


## Included Libraries / Components

* The **BME680 sensor** Element uses the Open Source from the Arduino Library published by Bosch Sensortec GmbH <https://github.com/BoschSensortec/BSEC-Arduino-library/tree/master/src/bme680> using a BSD 3-clause license.


