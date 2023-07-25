// HomeDing.h

// default settings of the HomeDing CPP defines and Element registrations.

#pragma once

// #include <hdProfile.h>

#include <Board.h>            // Platform
#include <Element.h>          // Abstract Elements
#include <ElementRegistry.h>  // Element Registry

#include <core/Logger.h>

// enable all TRACE at once here
// #define TRACE(...) LOGGER_ETRACE(__VA_ARGS__)

// some common property names

extern const char *PROP_VALUE;
extern const char *PROP_PIN;
extern const char *PROP_ADDRESS;

extern const char *ACTION_ONVALUE;
extern const char *ACTION_ONTEMPERATURE;
extern const char *ACTION_ONHUMIDITY;
extern const char *ACTION_ONPRESSURE;

// global access to the board implementation
extern Board homeding;

// ===== SYS and Common Elements =====

// These elements that are required in any case add themselves to the ElementRegistry.
// They are registered into the ElementRegistry using the initialization of ::registered in the *.cpp files:
//
// ===== SYSTEM Elements =====

// The only strictly required elements are: DeviceElement and OTAElement. They are included any time.
// The other system level elements SSDPElement, TimeElement

#ifdef HOMEDING_INCLUDE_ALL

#define HOMEDING_INCLUDE_SYSTEM  // all system elements
#define HOMEDING_INCLUDE_CORE
#define HOMEDING_INCLUDE_TM1637
#define HOMEDING_INCLUDE_PMS
#define HOMEDING_INCLUDE_WEATHERFEED

#endif



// ===== CORE Elements =====

// The Core Elements that are very common and add themselves to the ElementRegistry
// when the corresponding HOMEDING_INCLUDE_Xxxx macro is defined.
// Easy include of all elements that do not require a separate library
// by defining HOMEDING_INCLUDE_CORE.

// The MACRO HOMEDING_REGISTER set to 1 and the full list of HOMEDING_INCLUDE_Xxxx defines must exist only once
// and is done in the main sketch file to allow small and full configurations.
// See <ConfigureSketchElements.md>

#ifdef HOMEDING_INCLUDE_CORE

// all system elements
#define HOMEDING_INCLUDE_SYSTEM

// most often used Elements
#define HOMEDING_INCLUDE_Value
#define HOMEDING_INCLUDE_Button
#define HOMEDING_INCLUDE_Switch
#define HOMEDING_INCLUDE_Analog
#define HOMEDING_INCLUDE_DigitalIn
#define HOMEDING_INCLUDE_DigitalSignal
#define HOMEDING_INCLUDE_Touch
#define HOMEDING_INCLUDE_DigitalOut
#define HOMEDING_INCLUDE_PWMOut

#define HOMEDING_INCLUDE_AND
#define HOMEDING_INCLUDE_OR
#define HOMEDING_INCLUDE_ADD
#define HOMEDING_INCLUDE_SCENE
#define HOMEDING_INCLUDE_SELECT
#define HOMEDING_INCLUDE_REFERENCE
#define HOMEDING_INCLUDE_Timer
#define HOMEDING_INCLUDE_Schedule
#define HOMEDING_INCLUDE_Alarm
#define HOMEDING_INCLUDE_MAP

#define HOMEDING_INCLUDE_LOG
#define HOMEDING_INCLUDE_REMOTE
#endif

#ifdef HOMEDING_INCLUDE_SYSTEM
// DeviceElement and OTAElement are included in any case.
#define HOMEDING_INCLUDE_NTPTIME
#define HOMEDING_INCLUDE_Time
#endif

// system elements only in full firmware
#ifdef HOMEDING_INCLUDE_FULL_SYSTEM
#define HOMEDING_INCLUDE_SSDP
#define HOMEDING_INCLUDE_RTCSTATE
#endif

// Easy include of all elements for an attached display.
// by defining HOMEDING_INCLUDE_DISPLAY

#ifdef HOMEDING_INCLUDE_DISPLAY
#define HOMEDING_INCLUDE_DisplayText
#define HOMEDING_INCLUDE_DisplayDot
#define HOMEDING_INCLUDE_DisplayLine
#define HOMEDING_INCLUDE_DisplayButton
#endif

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

#ifdef HOMEDING_INCLUDE_OR
#include <OrElement.h>
#endif

#ifdef HOMEDING_INCLUDE_ADD
#include <AddElement.h>
#endif

#ifdef HOMEDING_INCLUDE_REFERENCE
#include <ReferenceElement.h>
#endif

#ifdef HOMEDING_INCLUDE_Analog
#include <AnalogElement.h>
#endif

#ifdef HOMEDING_INCLUDE_MAP
#include <MapElement.h>
#endif

#ifdef HOMEDING_INCLUDE_RTCSTATE
#include <RTCStateElement.h>
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

#if defined(HOMEDING_INCLUDE_Touch) && defined(ESP32) 
#include <TouchElement.h>
#endif

#ifdef HOMEDING_INCLUDE_DigitalOut
#include <DigitalOutElement.h>
#endif

#ifdef HOMEDING_INCLUDE_PWMOut
#include <PWMOutElement.h>
#endif

#ifdef HOMEDING_INCLUDE_SCENE
#include <SceneElement.h>
#endif

#ifdef HOMEDING_INCLUDE_SELECT
#include <SelectElement.h>
#endif

#ifdef HOMEDING_INCLUDE_LOG
#include <core/LogElement.h>
#endif

#if defined(HOMEDING_INCLUDE_PMS) && defined(ESP8266)
#include <sensors/PMSElement.h>
#endif

#ifdef HOMEDING_INCLUDE_REMOTE
#include <RemoteElement.h>
#endif

#ifdef HOMEDING_INCLUDE_MQTT
#include <MQTTElement.h>
#endif

#ifdef HOMEDING_INCLUDE_WEATHERFEED
#include <WeatherFeedElement.h>
#endif


// ===== OPTIONAL DISPLAY ELEMENTS =====

#ifdef HOMEDING_INCLUDE_DISPLAYLCD
#include <displays/DisplayLCDElement.h>
#endif

#ifdef HOMEDING_INCLUDE_DISPLAYMAX7219
#include <displays/DisplayMAX7219Element.h>
#endif

#ifdef HOMEDING_INCLUDE_DISPLAYSSD1306
#include <displays/DisplaySSD1306Element.h>
#endif

#ifdef HOMEDING_INCLUDE_DISPLAYSH1106
#include <displays/DisplaySH1106Element.h>
#endif

#ifdef HOMEDING_INCLUDE_DISPLAYST7735
#include <displays/DisplayST7735Element.h>
#endif

#ifdef HOMEDING_INCLUDE_DISPLAYST7789
#include <displays/DisplayST7789Element.h>
#endif

#ifdef HOMEDING_INCLUDE_DISPLAYST7796
#include <displays/DisplayST7796Element.h>
#endif

#ifdef HOMEDING_INCLUDE_DisplayText
#include <displays/DisplayTextElement.h>
#endif

#ifdef HOMEDING_INCLUDE_DisplayDot
#include <displays/DisplayDotElement.h>
#endif

#ifdef HOMEDING_INCLUDE_DisplayLine
#include <displays/DisplayLineElement.h>
#endif

#ifdef HOMEDING_INCLUDE_DisplayButton
#include <displays/DisplayButtonElement.h>
#endif

#ifdef HOMEDING_INCLUDE_TM1637
#include <TM1637Element.h>
#endif

// ===== LIGHT ELEMENTS =====

#ifdef HOMEDING_INCLUDE_LIGHT
#include <light/LightElement.h>
#endif

#ifdef HOMEDING_INCLUDE_COLOR
#include <light/ColorElement.h>
#endif

#ifdef HOMEDING_INCLUDE_NEOPIXEL
#include <light/NeoElement.h>
#endif

#ifdef HOMEDING_INCLUDE_APA102
#include <light/APA102Element.h>
#endif

#ifdef HOMEDING_INCLUDE_MY9291
#include "light/MY9291Element.h"
#endif

#ifdef HOMEDING_INCLUDE_P9813
#include "light/P9813Element.h"
#endif


// ===== OPTIONAL AUDIO ELEMENTS =====

#ifdef HOMEDING_INCLUDE_TONE
#include "audio/ToneElement.h"
#endif


// ===== OPTIONAL ELEMENTS =====

#ifdef HOMEDING_INCLUDE_DHT
#include <sensors/DHTElement.h>
#endif

#ifdef HOMEDING_INCLUDE_AM2320
#include <sensors/AM2320Element.h>
#endif

#ifdef HOMEDING_INCLUDE_AHT20
#include <sensors/AHT20Element.h>
#endif

#ifdef HOMEDING_INCLUDE_SHT20
#include <sensors/SHT20Element.h>
#endif

#ifdef HOMEDING_INCLUDE_SCD4X
#include <sensors/SCD4XElement.h>
#endif

#ifdef HOMEDING_INCLUDE_RFCODES
#include <RFCodesElement.h>
#endif

#ifdef HOMEDING_INCLUDE_ROTARY
#include <RotaryElement.h>
#endif

#ifdef HOMEDING_INCLUDE_DALLAS
#include <sensors/DallasElement.h>
#endif

#ifdef HOMEDING_INCLUDE_BMP280
#include <sensors/BMP280Element.h>
#endif

#ifdef HOMEDING_INCLUDE_BME680
#include <sensors/BME680Element.h>
#endif

#ifdef HOMEDING_INCLUDE_BL0937
#include <BL0937Element.h>
#endif

#ifdef HOMEDING_INCLUDE_BH1750
#include <sensors/BH1750Element.h>
#endif

#ifdef HOMEDING_INCLUDE_MAX7219
#include <MAX7219Element.h>
#endif

#if defined(HOMEDING_INCLUDE_SDMMC) && defined(ESP32)
#include <SDMMCElement.h>
#endif

#if defined(HOMEDING_INCLUDE_SD)
#include <SDElement.h>
#endif

#ifdef HOMEDING_INCLUDE_MENU
#include <MenuElement.h>
#endif

#if defined(HOMEDING_INCLUDE_SSDP)
#include <core/SSDPElement.h>
#endif
