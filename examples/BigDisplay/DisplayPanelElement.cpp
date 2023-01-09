/**
 * @file DisplayPanelElement.cpp
 * @brief Display Element for Panel compatible TFT displays.
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
 * Changelog:see DisplayPanelElement.h
 */

#include <Arduino.h>
#include <Board.h>
#include <HomeDing.h>

#include "DisplayPanelElement.h"
#include "DisplayPanelAdapter.h"

#define TRACE(...) // LOGGER_EINFO(__VA_ARGS__)

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new DisplayPanelElement
 * @return DisplayPanelElement* created element
 */
Element *DisplayPanelElement::create() {
  return (new DisplayPanelElement());
}  // create()


/* ===== Element functions ===== */

// All required parameters are handled by DisplayElement::set()

/**
 * @brief Activate the DisplayPanelElement and register a Display Adapter to LCD
 * in the board.
 */
void DisplayPanelElement::start() {
  TRACE("start()");

  DisplayAdapter *d = new DisplayPanelAdapter();
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
