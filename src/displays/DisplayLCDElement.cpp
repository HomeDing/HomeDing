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
#include <Board.h>
#include <HomeDing.h>

#include <displays/DisplayLCDElement.h>

#include <displays/DisplayAdapterLCD.h>

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new DisplayLCDElement
 * @return DisplayLCDElement* created element
 */
Element *DisplayLCDElement::create()
{
  return (new DisplayLCDElement());
} // create()


/* ===== Element functions ===== */

DisplayLCDElement::DisplayLCDElement()
{
  startupMode = Element_StartupMode::System;
  // standards for LCD on I2C
  _address = 0x27;
  _height = 2;
  _width = 16;
}

/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool DisplayLCDElement::set(const char *name, const char *value)
{
  bool ret = DisplayElement::set(name, value);
  return (ret);
} // set()


/**
 * @brief Activate the DisplayLCDElement and register a Display Adapter to LCD
 * in the board.
 */
void DisplayLCDElement::start()
{
  // TRACE("start()");
  DisplayAdapter *d = new DisplayAdapterLCD(_address, _height, _width);

  bool success = d->init(_board);
  if (success) {
    _board->display = d;
    DisplayElement::start();

  } else {
    LOGGER_EERR("no display found.");
    delete d;
  } // if
} // start()

// End
