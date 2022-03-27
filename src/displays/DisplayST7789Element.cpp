/**
 * @file DisplayST7789Element.cpp
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
 * Changelog:see DisplayST7789Element.h
 */

#include <Arduino.h>
#include <Board.h>
#include <HomeDing.h>

#include <displays/DisplayST7789Element.h>

#include <displays/DisplayAdapterST7789.h>

#define TRACE(...) LOGGER_EINFO(__VA_ARGS__)

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
  // config.spiCS = 5;
  // config.spiDC = 16;
  // config.spiRST = 23;
  // config.spiMOSI = 19;
  // config.spiMISO = -1;
  // config.spiCLK = 18;

  DisplayAdapter *d = new DisplayAdapterST7789();
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
