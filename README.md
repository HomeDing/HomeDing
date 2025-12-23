# The HomeDing Library

This is a Arduino library to build small IoT devices without the need for cloud services or a local central device.
It includes capabilities that can be combined by configuration providing a flexible low code solution.

[![Arduino Library Format Check](https://github.com/HomeDing/HomeDing/actions/workflows/checkLibraryFormat.yml/badge.svg)](https://github.com/HomeDing/HomeDing/actions/workflows/checkLibraryFormat.yml)
[![Build Examples for ESP32](https://github.com/HomeDing/HomeDing/actions/workflows/buildESP32.yml/badge.svg)](https://github.com/HomeDing/HomeDing/actions/workflows/buildESP32.yml)
[![arduino-library-badge](https://www.ardu-badge.com/badge/HomeDing.svg?)](https://www.ardu-badge.com/HomeDing)

The library includes a complete ecosystem to build a IoT device by combining IoT specific Elements that interact using events and actions.
These elements and the data flow and activities between elements can be configured without compiling a new firmware.
It is extensible by adding new specific Elements.

The devices run stand-alone only connected to the local WiFi and provides
information and control of the device using a built-in web server and web services.

Documentation can be found in at [HomeDing documentation site](https://homeding.github.io)

This Library is published using a [BSD 3-clause license](./LICENSE).


## Documentation

The Homeding library is documented on <https://homeding.github.io>.

The library has implemented many
[Elements](https://homeding.github.io/elements/index.htm) for simple Input and Output of signals,
[Sensors](https://homeding.github.io/sensors/sensors.htm),
[Displays](https://homeding.github.io/displays/index.htm), lightning, logical processing, time support and many more.

The embedded Web Server enables configuration and control of the device.

It supports:

* a device specific UI that can be accessed by browsers and mobile devices
* web services using the REST format to control the device
* web services using the REST format to get the status of the device
* retrieving actions from other devices
* a small IDE (Integrated Development Environment) that allows changing the configuration

More documentation can be found in the [HomeDing documentation site](https://homeding.github.io)

## Supported CPUs

The library supports the following CPUs:

* **ESP8266** with boards of 1MB and 4MB flash memory
* **ESP32** Wrover SoC
* **ESP32** Wroom SoC with PSRAM for Audio processing
* **ESP32-C3**

More will follow as the library has no strong dependencies on the processor used.

**Important** -- The SSDP discovery library for ESP32 must be installed manually
as it is not listed in the Arduino Library Manager.
It can be found at <https://github.com/luc-github/ESP32SSDP>
(download zip file and extract into the libraries folder or `git clone https://github.com/luc-github/ESP32SSDP`)


## Provided examples

The HomeDing library can be used for a broad variety of IoT devices partially shown by the
provided examples.

**standard** --
Full featured firmware containing all most-used stable elements like standard I/O, sensors,
light drivers and displays. Runs well on a 4MB Flash with all supported processors.

**plug** and **bulb** -- 
These [Minimal Examples](https://github.com/HomeDing/HomeDing/tree/master/examples/minimal)
are implemented to support ESP8266 based devices with 1MB Flash like switches and lights.
They contain a reduced set of elements for the specific use-case and a small footprint Web-UI.
These work well with small self-made and off-the-shell devices.

**DevDing** --
An extended standard setup that includes hints and tools for development of new elements.

**WebRadio** --
The [WebRadio example](https://github.com/HomeDing/HomeDing/tree/master/examples/webradio) is
implemented especially for the ESP32 Wroover with PSRAM to build a Web Radio by using an Audio
library. This Web Radio can be controlled through the embedded web interface.

**micro** --
This is not a real example but is used to flash small devices with firmware for specific
conditions. See [micro example readme](/examples/micro/README.md)

**RFBridge** --
The [RFBridge Example](https://homeding.github.io/examples/rfbridge.htm)** shows how to
implement a RF433 bridge device sending and receiving 433MHz signals.

**radio** --
The [Radio example](https://github.com/HomeDing/HomeDing/tree/master/examples/radio) has a
local Element to control the features of the Arduino [Radio
Library](https://github.com/mathertel/Radio) that to ontrol many Radio FM receiver chips.
The [Radio Element](https://homeding.github.io/elements/audio/radio.htm) is avaiable as part of
this example.

**probe** --
The Probe Example shows how to implement special, local elements that can retrieve sensor data
from INA219 and INA266 current and voltage sensors to build a low-voltage power consumption
logging device.
It can be used with any board that has a 4MByte Flash memory like the NodeMCU boards.

<!--
DashButton
WordClock
-->


## BSD License

The HomeDing library is open-source and uses one of the most permissive licenses so you can use it for any project:

* Private use
* Commercial use
* You can modify
* You can distribute

If you would like to contribute by additions or bug fixes you are welcome to open issues or pull requests on Github.

If you plan to use it in a commercial product you are welcome to contact me regarding working and planning the future.


## Included Libraries / Components

* The **BME680 sensor** Element uses the Open Source from the Arduino Library published by Bosch Sensortec GmbH <https://github.com/BoschSensortec/BSEC-Arduino-library/tree/master/src/bme680> using a BSD 3-clause license.

## See also

* [CLI for Homeding](https://homeding.github.io)
