// HomeDing.h

// default settings of the HomeDing CPP defines and Element registrations.

#ifndef HOMEDING_H
#define HOMEDING_H

#include <Arduino.h>

// #define LOGGER_MODULE ""

// #ifdef DEBUG_ESP_PORT
// #define LOGGER_ENABLE_TRACE
// #endif

// This is the set of Elements of the HomeDing Library
// that do not need any specific library to be installed.

// #define HOMEDINGUSE_Button


#include "Element.h"
#include "ElementRegistry.h"

// ===== CORE Elements =====

// Elements that are required in any case add themselves to the ElementRegistry
// See SSDPElement.cpp or DeviceElement.cpp as examples.

// easy include of all libraries that do not require a separate library


// ===== CORE Elements =====

// The Core Elements that are required in any case add themselves to the ElementRegistry.
// There are no HOMEDING_INCLUDE_ defines for these Elements:
// Device, SSDP, OTA


// See SSDPElement.cpp or DeviceElement.cpp as examples.

// easy include of all libraries that do not require a separate library

#ifdef HOMEDING_INCLUDE_CORE

#define HOMEDING_INCLUDE_Value
#define HOMEDING_INCLUDE_Button
#define HOMEDING_INCLUDE_Timer
#define HOMEDING_INCLUDE_DigitalOut
#define HOMEDING_INCLUDE_DisplayText
#define HOMEDING_INCLUDE_DisplayDot

#endif


// Register the Elements by inculing the definition wiht defined HOMEDING_REGISTER

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

#ifdef HOMEDING_INCLUDE_DigitalOut
#include <DigitalOutElement.h>
#endif

#ifdef HOMEDING_INCLUDE_DisplayText
#include <DisplayTextElement.h>
#endif

#ifdef HOMEDING_INCLUDE_DisplayDot
#include <DisplayDotElement.h>
#endif

// ===== OPTIONAL ELEMENTS =====

#ifdef HOMEDING_INCLUDE_DHT
#include <DHTElement.h>
#endif

#endif
