/**
 * @file DisplayESP32PanelElement.cpp
 *
 * @brief Display Element for TFT displays using the ESP32-S3 specific
 * 16-bit Panel Data interface.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license.
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:see DisplayESP32PanelElement.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#if defined(ESP32) && defined(CONFIG_IDF_TARGET_ESP32S3)

#include "displays/DisplayESP32PanelElement.h"

#include "displays/DisplayESP32PanelAdapter.h"

#define TRACE(...)  // LOGGER_EINFO(__VA_ARGS__)

/* ===== Static factory function ===== */

/// @brief static factory function to create a new DisplayESP32PanelElement
/// @return DisplayESP32PanelElement* created element
Element *DisplayESP32PanelElement::create() {
  return (new DisplayESP32PanelElement());
}  // create()


/* ===== Element functions ===== */

/// @brief Activate the DisplayESP32PanelElement and register a Display Adapter to LCD
void DisplayESP32PanelElement::start() {
  DisplayElement::start(new DisplayESP32PanelAdapter());
}  // start()

#endif

// End
