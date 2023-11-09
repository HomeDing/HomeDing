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
#include <HomeDing.h>

#include "DisplaySH1106Element.h"

#include <displays/DisplaySH1106Adapter.h>

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

/// @brief Activate the DisplaySH1106Element and register
/// the Display Adapter to the board.
void DisplaySH1106Element::start() {
  LOGGER_ETRACE("start()");
  DisplayAdapter *d = new DisplaySH1106Adapter();

  if (d->setup(_board, &config)) {
    if (d->start()) {
      _board->display = d;
      DisplayElement::start();

    } else {
      LOGGER_EERR("no display found");
      delete d;
      active = false;
    }  // if
  }    // if
}  // start()

// End
