/**
 * @file DisplaySH1106Element.cpp
 * @brief Display Element for SH1106 based OLED displays.
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
 * Changelog:see DisplaySH1106Element.h
 */

#include <Arduino.h>
#include <Board.h>
#include <HomeDing.h>

#include "DisplaySH1106Element.h"

#include <displays/DisplayAdapterSH1106.h>

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new DisplaySH1106Element
 * @return DisplaySH1106Element* created element
 */
Element *DisplaySH1106Element::create() {
  return (new DisplaySH1106Element());
}  // create()


/* ===== Element functions ===== */

// All required parameters are handled by DisplayElement::set()

/**
 * @brief Activate the DisplaySH1106Element and register a Display Adapter to
 * LCD in the board.
 */
void DisplaySH1106Element::start() {
  DisplayElement::start();

  // TRACE("start()");
  DisplayAdapter *d = new DisplayAdapterSH1106();

  if (d->setup(_board, &config)) {
    if (d->start()) {
      _board->display = d;

    } else {
      LOGGER_EERR("no display found.");
      delete d;
      active = false;
    }  // if
  }    // if
}  // start()

// End
