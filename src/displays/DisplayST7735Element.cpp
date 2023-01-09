/**
 * @file DisplayST7735Element.cpp
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
 * Changelog:see DisplayST7735Element.h
 */

#include <Arduino.h>
#include <Board.h>
#include <HomeDing.h>

#include <displays/DisplayST7735Element.h>
#include <displays/DisplayST7735Adapter.h>

#define TRACE(...) // LOGGER_EINFO(__VA_ARGS__)

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new DisplayST7735Element
 * @return DisplayST7735Element* created element
 */
Element *DisplayST7735Element::create() {
  return (new DisplayST7735Element());
}  // create()


/* ===== Element functions ===== */

// All required parameters are handled by DisplayElement::set()

/**
 * @brief Activate the DisplayST7735Element and register a Display Adapter to LCD
 * in the board.
 */
void DisplayST7735Element::start() {
  TRACE("start()");

  DisplayAdapter *d = new DisplayST7735Adapter();
  if (d->setup(_board, &config)) {
    bool success = d->start();
    if (success) {
      _board->display = d;
      DisplayElement::start();

    } else {
      LOGGER_EERR("no display found");
      delete d;
    }  // if
  }    // if
}  // start()

// End
