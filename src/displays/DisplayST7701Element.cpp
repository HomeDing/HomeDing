/**
 * @file DisplayST7701Element.cpp
 *
 * @brief Display Element for ST7701 based TFT displays using the ESP32-S3 specific
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
 * Changelog:see DisplayST7701Element.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include "displays/DisplayST7701Element.h"

#include "displays/DisplayESP32PanelAdapter.h"
#include "DisplayST7701Element.h"

#define TRACE(...)  // LOGGER_EINFO(__VA_ARGS__)

/* ===== Static factory function ===== */

/// @brief static factory function to create a new DisplayST7701Element
/// @return DisplayST7701Element* created element
Element *DisplayST7701Element::create() {
  return (new DisplayST7701Element());
}  // create()


/* ===== Element functions ===== */

/// @brief Activate the DisplayST7701Element and register a Display Adapter to LCD
void DisplayST7701Element::start() {
  DisplayElement::start(new DisplayESP32PanelAdapter());
}  // start()

// End
