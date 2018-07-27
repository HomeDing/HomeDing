# IoT Board System

The IoT Board System enables building IoT Devices using ESP8266 boards.

**This is WORK in PROGRESS - no first version yet.**

The IoT Board System enables building IoT Devices (here called Things in short) using ESP8266 based boards. It is a complete ecosystem of the required internal functionality of a Thing (called the Elements) together with a Web Server implementation that runs a small IDE (Integrated Development Environment) that allows changing and redefining the function of a thing by using a web browser only.

Especially it is running completely on it’s own and doesn’t require any cloud system and is only exchanging data defined by the configuration- (it optionally can interact with some of the Cloud systems)

## The overall picture

When building Web enabled devices and especially the smaller things that act as a source of a specific information or handles some local automation there is always some minimum of functionality required to get the device up and running. This is divided into the general (one time) implementation within the Board class and IO specific implementation in the Elements.

The Board class that has the role to create and initialize all Elements, manage all active elements and allow the communication between elements without hard dependencies.

The config class is required to bootstrap all the elements and provide the initial values.

The major diversity of functionality is implemented into the element classes that all share the same implementation idea and interface that is flexible enough to build more complex logic by offering basis elements with their typical functionality and applying applying different configurations at startup.

The display is one of the components that exists only once but is used by different Elements. These types of classes, often called singletons are created by the board and are available to all Elements.

![Overview](https://www.mathertel.de/Arduino/Board/ComponentOverview.png "Overview")


## Implemented Elements

* **Button Element**

  This Element implement using a momentary button on a digital in pin to toggle between on and off by pressing the button.

* **Device Element**

  This Element controls some of the principal settings of the board like networkname and automatic reboot.

* **DHT Element**

  This Element implements reading temperature and humidity from DHT type sensors.

  Supporting DHT11, DHT22, AM2302 and RHT03.

* **DigitalOut Element**

  This Element implements using a digital ouput pin. Actions from other elements can be used to set the level. The simplest case here is using the buildin LED.

* **Display Element** 

  This Element implements displaying values and figures on a LED/LCD display.
  
  Supporting SSD1306.

* **Element**

  This Element is the base class of all Elements and implements common functionality to ease implemening more of them.

* **Remote Element**

  This Element forwards actions to remote devices e.g. allowing to physically separating a sensor from the display.

* **Timer Element**
 
  This Element implements an timer that can be used to trigger actions based on some time settings.
