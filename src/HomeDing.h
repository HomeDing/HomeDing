// HomeDing.h

// default settings of the HomeDing CPP defines and Element registrations.

#ifndef HOMEDING_H
#define HOMEDING_H

#include <Element.h>
#include <ElementRegistry.h>

// some common property names

extern const char *PROP_VALUE;
extern const char *PROP_DESCRIPTION;
extern const char *PROP_PIN;
extern const char *PROP_ADDRESS;

extern const char *PROP_INVERSE;
extern const char *PROP_PULLUP;
extern const char *PROP_DURATION;

extern const char *ACTION_ONVALUE;
extern const char *ACTION_ONHIGH;
extern const char *ACTION_ONLOW;
extern const char *ACTION_ONTEMPERATURE;
extern const char *ACTION_ONHUMIDITY;
extern const char *ACTION_ONPRESSURE;

// ===== SYS and Common Elements =====

// These elements that are required in any case add themselves to the ElementRegistry.
// They are registered into the ElementRegistry using the initialization of ::registered in the *.cpp files:
//
// ===== SYSTEM Elements =====

// The only strictly required elements are: DeviceElement and OTAElement. They are included any time.
// The other system level elements SSDPElement, TimeElement 

// ===== CORE Elements =====

// The Core Elements that are very common and add themselves to the ElementRegistry
// when the corresponding HOMEDING_INCLUDE_Xxxx macro is defined. 
// Easy include of all elements that do not require a separate library
// by defining HOMEDING_INCLUDE_CORE.

// The MACRO HOMEDING_REGISTER set to 1 and the full list of HOMEDING_INCLUDE_Xxxx defines must exist only once
// and is done in the main sketch file to allow small and full configurations.
// See <ConfigureSketchElements.md>

#ifdef HOMEDING_INCLUDE_CORE

#define HOMEDING_INCLUDE_SYSTEM // all system elements

#define HOMEDING_INCLUDE_Value
#define HOMEDING_INCLUDE_Button
#define HOMEDING_INCLUDE_Switch
#define HOMEDING_INCLUDE_Analog
#define HOMEDING_INCLUDE_DigitalIn
#define HOMEDING_INCLUDE_DigitalSignal
#define HOMEDING_INCLUDE_DigitalOut
#define HOMEDING_INCLUDE_PWMOut

#define HOMEDING_INCLUDE_AND
#define HOMEDING_INCLUDE_Timer
#define HOMEDING_INCLUDE_Schedule
#define HOMEDING_INCLUDE_Alarm

#define HOMEDING_INCLUDE_LOG

#define HOMEDING_INCLUDE_REMOTE
#endif

#ifdef HOMEDING_INCLUDE_SYSTEM
#define HOMEDING_INCLUDE_SSDP
#define HOMEDING_INCLUDE_NTPTIME
#define HOMEDING_INCLUDE_Time
#endif

// Easy include of all elements for an attached display.
// by defining HOMEDING_INCLUDE_DISPLAY

#ifdef HOMEDING_INCLUDE_DISPLAY
#define HOMEDING_INCLUDE_DisplayText
#define HOMEDING_INCLUDE_DisplayDot
#define HOMEDING_INCLUDE_DisplayLine
#endif

#include <core/logger.h>

// Register the Elements by including the definition with defined HOMEDING_REGISTER

#ifdef HOMEDING_INCLUDE_Value
#include <ValueElement.h>
#endif

#ifdef HOMEDING_INCLUDE_Button
#include <ButtonElement.h>
#endif

#ifdef HOMEDING_INCLUDE_Switch
#include <SwitchElement.h>
#endif

#ifdef HOMEDING_INCLUDE_AND
#include <AndElement.h>
#endif

#ifdef HOMEDING_INCLUDE_Analog
#include <AnalogElement.h>
#endif

// ===== Time related Elements =====

#ifdef HOMEDING_INCLUDE_Time
#include <time/TimeElement.h>
#endif

#ifdef HOMEDING_INCLUDE_Alarm
#include <time/AlarmElement.h>
#endif

#ifdef HOMEDING_INCLUDE_DSTIME
#include <time/DSTimeElement.h>
#endif

#ifdef HOMEDING_INCLUDE_NTPTIME
#include <time/NTPTimeElement.h>
#endif

#ifdef HOMEDING_INCLUDE_Timer
#include <time/TimerElement.h>
#endif

#ifdef HOMEDING_INCLUDE_Schedule
#include <time/ScheduleElement.h>
#endif


#ifdef HOMEDING_INCLUDE_DigitalIn
#include <DigitalInElement.h>
#endif

#ifdef HOMEDING_INCLUDE_DigitalSignal
#include <DigitalSignalElement.h>
#endif

#ifdef HOMEDING_INCLUDE_DigitalOut
#include <DigitalOutElement.h>
#endif

#ifdef HOMEDING_INCLUDE_PWMOut
#include <PWMOutElement.h>
#endif

#ifdef HOMEDING_INCLUDE_LOG
#include <core/LogElement.h>
#endif

#ifdef HOMEDING_INCLUDE_PMS
#include <sensors/PMSElement.h>
#endif

#ifdef HOMEDING_INCLUDE_REMOTE
#include <RemoteElement.h>
#endif

#ifdef HOMEDING_INCLUDE_WEATHERFEED
#include <WeatherFeedElement.h>
#endif

// ===== OPTIONAL DISPLAY ELEMENTS =====

#ifdef HOMEDING_INCLUDE_DISPLAYLCD
#include <displays/DisplayLCDElement.h>
#endif

#ifdef HOMEDING_INCLUDE_DISPLAYSSD1306
#include <displays/DisplaySSD1306Element.h>
#endif

#ifdef HOMEDING_INCLUDE_DISPLAYSH1106
#include <displays/DisplaySH1106Element.h>
#endif

#ifdef HOMEDING_INCLUDE_DisplayText
#include <DisplayTextElement.h>
#endif

#ifdef HOMEDING_INCLUDE_DisplayDot
#include <DisplayDotElement.h>
#endif

#ifdef HOMEDING_INCLUDE_DisplayLine
#include <DisplayLineElement.h>
#endif

// ===== OPTIONAL ELEMENTS =====

#ifdef HOMEDING_INCLUDE_DHT
#include <sensors/DHTElement.h>
#endif

#ifdef HOMEDING_INCLUDE_LIGHT
#include <LightElement.h>
#endif

#ifdef HOMEDING_INCLUDE_NEOPIXEL
#include <NeoElement.h>
#endif

#ifdef HOMEDING_INCLUDE_RFSend
#include <RFSendElement.h>
#endif

#ifdef HOMEDING_INCLUDE_ROTARY
#include <RotaryElement.h>
#endif

#ifdef HOMEDING_INCLUDE_DS18B20
#include <sensors/DS18B20Element.h>
#endif

#ifdef HOMEDING_INCLUDE_BMP280
#include <sensors/BMP280Element.h>
#endif

#ifdef HOMEDING_INCLUDE_BME680
#include <sensors/BME680Element.h>
#endif

#ifdef HOMEDING_INCLUDE_MENU
#include <MenuElement.h>
#endif

#ifdef HOMEDING_INCLUDE_SSDP
#include <core/SSDPElement.h>
#endif


#endif
