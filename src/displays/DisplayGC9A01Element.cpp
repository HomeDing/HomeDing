/**
 * @file DisplayGC9A01Element.cpp
 * @brief Display Element for GC9A01 compatible TFT displays.
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
 * Changelog:see DisplayGC9A01Element.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <displays/DisplayGC9A01Element.h>
#include <displays/DisplayGC9A01Adapter.h>
#include "DisplayGC9A01Element.h"

#define TRACE(...)  // LOGGER_EINFO(__VA_ARGS__)

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new DisplayGC9A01Element
 * @return DisplayGC9A01Element* created element
 */
Element *DisplayGC9A01Element::create() {
  return (new DisplayGC9A01Element());
}  // create()


/* ===== Element functions ===== */


/// @brief initialize a new Element.
/// @param board The board reference.
void DisplayGC9A01Element::init(Board *board) {
  TRACE("init()");
  DisplayElement::init(board);

  // set default values
  HomeDing::displayConfig.ips = true;
  HomeDing::displayConfig.width = 240;
  HomeDing::displayConfig.height = 240;
}


// All required parameters are handled by DisplayElement::set()


/// @brief Activate the DisplayGC9A01Element and register a Display Adapter to LCD
/// in the board.
void DisplayGC9A01Element::start() {
  DisplayElement::start(new DisplayGC9A01Adapter());
}  // start()


// End
