# The DevDing Example folder

This folder contains some implementations of a sketch and some Elements that are not as stable to be part of the standard library.
They are not compiled into the sketches that use the library.

The DevDing sketch is also used as a incubator project. In this folder you may find elements that have been implemented but for some reasons (e.g. unfinished documentation) are not yet mature and stable enough they will be moved into the library / src folder. 

See the short documentation in the *.h file for hints and purpose.
 
## Template Element 

This is an empty element that can be used as a template for implementing new elements.
All the virtual and overwritten methods that may be implemented are present in this implementation.
They may be removed in Element implementations when not required to reduce program size.

## Diag Element

This is an element that registers itself at system startup
and sends some diagnostic output to the Serial Console upon request:
* Reset Reason
* free heap memory
* i2c bus configuration
* scan i2c bus for devices
* dump content of RTC memory

## RFSend Element

A experimental Element for sending rf433 signals

## SerialCmd Element

Use the Serial input to create actions

## INA219Element.h

A experimental Element to control a INA219 power consumption chip.
