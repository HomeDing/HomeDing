// HomeDing.h

// default settings of the HomeDing CPP defines and Element registrations.

#ifndef HOMEDING_H
#define HOMEDING_H

#include <Arduino.h>

#include "Element.h"
#include <ElementRegistry.h>

// ===== SYS Elements =====

// Elements that are required in any case add themselves to the ElementRegistry
// See SSDPElement.cpp or DeviceElement.cpp as examples.

// easy include of all libraries that do not require a separate library


// ===== CORE Elements =====

// The Core Elements that are required in any case add themselves to the ElementRegistry.
// There are no HOMEDING_INCLUDE_ defines for these Elements:
// Device, SSDP, OTA
// because they are required for every HomeDing device.

// Easy include of all elements that do not require a separate library
// by defining HOMEDING_INCLUDE_CORE

#ifdef HOMEDING_INCLUDE_CORE

#define HOMEDING_INCLUDE_Value
#define HOMEDING_INCLUDE_Button
#define HOMEDING_INCLUDE_Timer
#define HOMEDING_INCLUDE_Schedule
#define HOMEDING_INCLUDE_DigitalOut
#define HOMEDING_INCLUDE_PWMOut
#define HOMEDING_INCLUDE_DisplayText
#define HOMEDING_INCLUDE_DisplayDot

#endif

#include <core/logger.h>

// Register the Elements by including the definition with defined HOMEDING_REGISTER

#define HOMEDING_REGISTER 1

#ifdef HOMEDING_INCLUDE_Value
#include <ValueElement.h>
#endif

#ifdef HOMEDING_INCLUDE_Button
#include <ButtonElement.h>
#endif

#ifdef HOMEDING_INCLUDE_Timer
#include <TimerElement.h>
#endif

#ifdef HOMEDING_INCLUDE_Schedule
#include <ScheduleElement.h>
#endif

#ifdef HOMEDING_INCLUDE_DigitalOut
#include <DigitalOutElement.h>
#endif

#ifdef HOMEDING_INCLUDE_PWMOut
#include <PWMOutElement.h>
#endif

#ifdef HOMEDING_INCLUDE_DisplayText
#include <DisplayTextElement.h>
#endif

#ifdef HOMEDING_INCLUDE_DisplayDot
#include <DisplayDotElement.h>
#endif

// ===== OPTIONAL DISPLAY ELEMENTS =====

#ifdef HOMEDING_INCLUDE_DISPLAYLCD
#include <displays/DisplayLCDElement.h>
#endif

#ifdef HOMEDING_INCLUDE_DISPLAYSSD1306
#include <displays/DisplaySSD1306Element.h>
#endif

// ===== OPTIONAL ELEMENTS =====

#ifdef HOMEDING_INCLUDE_DHT
#include <DHTElement.h>
#endif

#ifdef HOMEDING_INCLUDE_RFSend
#include <RFSendElement.h>
#endif

#ifdef HOMEDING_INCLUDE_ROTARY
#include <RotaryElement.h>
#endif

#endif
