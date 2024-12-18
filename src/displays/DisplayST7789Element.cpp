/**
 * @file DisplayST7789Element.cpp
 * @brief Display Element for ST7789 compatible TFT displays.
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
 * Changelog:see DisplayST7789Element.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <displays/DisplayST7789Element.h>
#include <displays/DisplayST7789Adapter.h>

#define TRACE(...)  // LOGGER_EINFO(__VA_ARGS__)

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new DisplayST7789Element
 * @return DisplayST7789Element* created element
 */
Element *DisplayST7789Element::create() {
  return (new DisplayST7789Element());
}  // create()


/* ===== Element functions ===== */

// All required parameters are handled by DisplayElement::set()


/// @brief Setup the element so it can be started and stopped.
void DisplayST7789Element::setup() {
  TRACE("setup()");
  DisplayElement::setup();
};



/// @brief Activate the Element and register a Display Adapter
/// in the board.
void DisplayST7789Element::start() {
  DisplayElement::start(new DisplayST7789Adapter());
}  // start()

// End
