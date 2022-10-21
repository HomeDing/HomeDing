/**
 * @file DisplaySSD1306Element.cpp
 * @brief Display Element for SSD1306 compatible LCD displays.
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
 * Changelog:see DisplaySSD1306Element.h
 */

#include <Arduino.h>
#include <Board.h>
#include <HomeDing.h>

#include <displays/DisplaySSD1306Element.h>

#include <displays/DisplayAdapterSSD1306.h>

#define TRACE(...) LOGGER_EINFO(__VA_ARGS__)

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new DisplaySSD1306Element
 * @return DisplaySSD1306Element* created element
 */
Element *DisplaySSD1306Element::create() {
  return (new DisplaySSD1306Element());
}  // create()


/* ===== Element functions ===== */

// All required parameters are handled by DisplayElement::set()

/**
 * @brief Activate the DisplaySSD1306Element and register a Display Adapter to LCD
 * in the board.
 */
void DisplaySSD1306Element::start() {
  TRACE("start()");

  DisplayAdapter *d = new DisplayAdapterSSD1306();
  if (d->setup(_board, &config)) {
    bool success = d->start();
    if (success) {
      _board->display = d;
      DisplayElement::start();

    } else {
      LOGGER_EERR("no display found.");
      delete d;
    }  // if
  }    // if
}  // start()

// End
