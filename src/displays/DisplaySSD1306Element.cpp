/**
 * @file DisplaySSD1306Element.cpp
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
 * Changelog:see DisplaySSD1306Element.h
 */

#include <Arduino.h>
#include <Element.h>
#include <Board.h>

#include "DisplaySSD1306Element.h"
#include <ElementRegistry.h>

#include <displays/DisplayAdapterSSD1306.h>

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new DisplaySSD1306Element
 * @return DisplaySSD1306Element* created element
 */
Element *DisplaySSD1306Element::create()
{
  return (new DisplaySSD1306Element());
} // create()


/* ===== Element functions ===== */

/**
 * @brief Constructor of a new DisplaySSD1306Element.
 */
DisplaySSD1306Element::DisplaySSD1306Element()
{
  startupMode = Element_StartupMode::System;
}


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool DisplaySSD1306Element::set(const char *name, const char *value)
{
  LOGGER_ETRACE("set(%s, %s)", name, value);
  bool ret = true;

  if (_stricmp(name, "address") == 0) {
    _address = _atoi(value);

  } else if (_stricmp(name, "sda") == 0) {
    _sda = _atopin(value);

  } else if (_stricmp(name, "scl") == 0) {
    _scl = _atopin(value);

  } else if (_stricmp(name, "resetpin") == 0) {
    _resetpin = _atopin(value);

  } else if (_stricmp(name, "height") == 0) {
    _height = _atoi(value);

  } else {
    ret = Element::set(name, value);
  } // if

  return (ret);
} // set()


/**
 * @brief Activate the DisplaySSD1306Element and register a Display Adapter to
 * LCD in the board.
 */
void DisplaySSD1306Element::start()
{
  DisplayAdapter *d;
  LOGGER_ETRACE("start(0x%x,%d,%d)", _address, _sda, _scl);

  // reset of the display is available on GPIO
  if (_resetpin >= 0) {
    pinMode(_resetpin, OUTPUT);
    digitalWrite(_resetpin, LOW); // turn low to reset OLED
    delay(50);
    digitalWrite(_resetpin, HIGH); // while OLED is running, must set high
  } // if

  d = (DisplayAdapter *)(new DisplayAdapterSSD1306(_address, _sda, _scl,
                                                   _height));

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
