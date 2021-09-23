# The Probe Example folder

This folder contains a implementations of a sketch and extra Elements to build a power consumption monitoring device
using the HomeDing library.

## Components and Wiring

To build the example you need a ESP8266 with 4MByte Flash Memory, an OLED display and a current sensot of type INA266 or INA 219.

The display and sensor are connected to the board using the I2C bus.

A more detailed description can be found in the HomeDing documentation:
<https://homeding.github.io/#page=/examples/probe.md>

### Files

-   **Probe.ino** - Main sketch
-   **README.md** - This file
-   **secrets.h** - network details when NOT using the [WiFi Manager](https://homeding.github.io/#page=/stepsnewdevice.md).
-   **upload.bat** - command file for windows to upload new firmware using OTA
-   **INA219Element.h** - Element interface for INA219 sensors 
-   **INA219Element.cpp** - Element implementation for INA219 sensors
-   **INA226Element.cpp** - Element interface for INA226 sensors
-   **INA226Element.h** - Element implementation for INA226 sensors


### Directories

-   **data** - example configurations (need to be renamed)

