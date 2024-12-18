/**
 * @file DisplayLCDElement.cpp
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
 * Changelog:see DisplayLCDElement.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <displays/DisplayLCDElement.h>

#include <displays/DisplayLCDAdapter.h>

#define TRACE(...)  // LOGGER_EINFO(__VA_ARGS__)

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new DisplayLCDElement
 * @return DisplayLCDElement* created element
 */
Element *DisplayLCDElement::create() {
  return (new DisplayLCDElement());
}  // create()


/* ===== Element functions ===== */

DisplayLCDElement::DisplayLCDElement() {
  TRACE("init()");
  DisplayElement::init(_board);

  HomeDing::displayConfig.i2cAddress = 0x27;
  HomeDing::displayConfig.height = 2;
  HomeDing::displayConfig.width = 16;
}

// All required parameters are handled by DisplayElement::set()

/**
 * @brief Activate the DisplayLCDElement and register a Display Adapter to LCD
 * in the board.
 */
void DisplayLCDElement::start() {
  DisplayElement::start(new DisplayLCDAdapter());
}  // start()

// End
