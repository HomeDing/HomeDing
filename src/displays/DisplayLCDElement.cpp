/**
 * @file DisplayLCDElement.cpp
 * @brief Element Template class.
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
#include <Element.h>
#include <Board.h>

#include <displays/DisplayLCDElement.h>
#include <ElementRegistry.h>

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
}

/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool DisplayLCDElement::set(const char *name, const char *value)
{
  LOGGER_ETRACE("set(%s, %s)", name, value);
  bool ret = true;

  if (_stricmp(name, "address") == 0) {
    _address = strtol(value, nullptr, 0);

  } else if (_stricmp(name, "lines") == 0) {
    _lines = strtol(value, nullptr, 0);

  } else if (_stricmp(name, "columns") == 0) {
    _cols = strtol(value, nullptr, 0);

  } else {
    ret = Element::set(name, value);
  } // if

  return (ret);
} // set()


/**
 * @brief Activate the DisplayLCDElement and register a Display Adapter to LCD
 * in the board.
 */
void DisplayLCDElement::start()
{
  DisplayAdapter *d;
  LOGGER_ETRACE("start(0x%x)", _address);
  d = (DisplayAdapter *)(new DisplayAdapterLCD(_address, _lines, _cols));

  bool success = d->init();
  if (success) {
    _board->display = d;
    Element::start();

  } else {
    LOGGER_EERR("no display found.");
    delete d;
  } // if
} // start()

// End
