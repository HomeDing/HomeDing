/**
 * @file DisplayMAX7219Element.cpp
 * @brief Display Element for HD44780 compatible LCD displays.
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
 * Changelog:see DisplayMAX7219Element.h
 */

#include <Arduino.h>
#include <Board.h>
#include <HomeDing.h>

#include <displays/DisplayMAX7219Element.h>

#include <displays/DisplayAdapterMAX7219.h>

#if !defined(TRACE)
#define TRACE(...) // LOGGER_JUSTINFO(__VA_ARGS__)
#endif

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new DisplayMAX7219Element
 * @return DisplayMAX7219Element* created element
 */
Element *DisplayMAX7219Element::create() {
  return (new DisplayMAX7219Element());
}  // create()


/* ===== Element functions ===== */

// All required parameters are handled by DisplayElement::set()

/**
 * @brief Activate the DisplayMAX7219Element and register a Display Adapter to LCD
 * in the board.
 */
void DisplayMAX7219Element::start() {
  TRACE("start()");

  DisplayAdapter *d = new DisplayAdapterMAX7219();
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
