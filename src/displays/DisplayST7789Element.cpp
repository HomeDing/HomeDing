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

#define TRACE(...) // LOGGER_EINFO(__VA_ARGS__)

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

/**
 * @brief Activate the DisplayST7789Element and register a Display Adapter to LCD
 * in the board.
 */
void DisplayST7789Element::start() {
  TRACE("start()");

  DisplayAdapter *d = new DisplayST7789Adapter();
  if (d->setup(_board, &config)) {
    bool success = d->start();
    if (success) {
      _board->display = d;
    } else {
      delete d;
    }  // if
    DisplayElement::start();
  }    // if
}  // start()

// End
