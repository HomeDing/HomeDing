/**
 * @file DisplayST7796Element.cpp
 * @brief Display Element for ST7796 compatible TFT displays.
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
 * Changelog:see DisplayST7796Element.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include "displays/DisplayST7796Element.h"

#include "displays/DisplayST7796Adapter.h"

#define TRACE(...)  // LOGGER_EINFO(__VA_ARGS__)

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new DisplayST7796Element
 * @return DisplayST7796Element* created element
 */
Element *DisplayST7796Element::create() {
  return (new DisplayST7796Element());
}  // create()


/* ===== Element functions ===== */

void DisplayST7796Element::init(Board *board) {
  TRACE("init()");
  DisplayElement::init(board);

  // some default values for ST7796 displays
  HomeDing::displayConfig.width = 320;
  HomeDing::displayConfig.height = 480;
}

// All required parameters are handled by DisplayElement::set()

/**
 * @brief Activate the DisplayST7796Element and register a Display Adapter to LCD
 * in the board.
 */
void DisplayST7796Element::start() {
  DisplayElement::start(new DisplayST7796Adapter());
}  // start()

// End
