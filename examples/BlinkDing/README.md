# HomeDing Blink example

Blinking the LED or a NodeMCU board.

No other hardware required

The onboard LED will blink: 1 sec. on, 2 sec. off.

## Hardware Setup

Just a NodeMCU Board connected to the Arduio Environment.

## Software Setup

Setup the Board in the Arduino Environment:

* Board: Any NodeMCU board
* Flash Size: 4MB (1MB SPIFFS)
* Debug Port: Serial
* Debug Level: None
* LwIP Variant: v2 lower memory
* CPU Frequency: 80 MHz
* Upload Speed: 115200
* Erase Flash: Only Sketch
* Port: (you have to choose the right one).

The following steps need to be done in the environment:

* Verify and Upload the sketch
* Upload the data folder to the SPIFFS filesystem

## Elements used

* **Timer Element**

  The Timer Element is used to create the on and off actions and send them to the Digital Out Element and log output.

* **DigitalOut Element**

  This Element implements using a digital ouput pin.
  The Actions from the Timer Element is used to set the level ON and OFF. 
  Because the LED is on when output is LOW the logical output is inverted to the Physical output.

* **Device Element**

  This Element is used to define the name of the Ding on the network.

* **SSDP Element**

  This Element is used to broadcast the Ding on the network using SSDP Protocol.
