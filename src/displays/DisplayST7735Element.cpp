/**
 * @file displays/DisplayST7735Element.cpp
 * @brief Display Element for ST7735 compatible TFT displays.
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
 * Changelog:see <displays/DisplayST7735Element.h>
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <displays/DisplayST7735Element.h>
#include <displays/DisplayST7735Adapter.h>

#define TRACE(...)  LOGGER_EINFO(__VA_ARGS__)

/* ===== Static factory function ===== */

  /// @brief Factory function to create a DisplayST7735Element.
  /// @return created Element* 
Element *DisplayST7735Element::create() {
  return (new DisplayST7735Element());
}  // create()


/* ===== Element functions ===== */

// All required parameters are handled by DisplayElement::set()

/// @brief Activate the Element and register a Display Adapter to LCD in the board.
void DisplayST7735Element::start() {
  DisplayElement::start(new DisplayST7735Adapter());
}  // start()

// End
