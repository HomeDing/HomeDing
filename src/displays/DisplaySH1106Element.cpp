/**
 * @file DisplaySH1106Element.cpp
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
 * Changelog:see DisplaySH1106Element.h
 */

#include <Arduino.h>
#include <HomeDing.h>
#include <Board.h>

#include "DisplaySH1106Element.h"

#include <displays/DisplayAdapterSH1106.h>

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new DisplaySH1106Element
 * @return DisplaySH1106Element* created element
 */
Element *DisplaySH1106Element::create()
{
  return (new DisplaySH1106Element());
} // create()


/* ===== Element functions ===== */

/**
 * @brief Constructor of a new DisplaySH1106Element.
 */
DisplaySH1106Element::DisplaySH1106Element()
{
  startupMode = Element_StartupMode::System;
}


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool DisplaySH1106Element::set(const char *name, const char *value)
{
  bool ret = true;

  if (_stricmp(name, PROP_ADDRESS) == 0) {
    _address = atoi(value);

  } else if (_stricmp(name, "resetpin") == 0) {
    _resetpin = _atopin(value);

  } else if (_stricmp(name, "height") == 0) {
    _height = atoi(value);

  } else {
    ret = Element::set(name, value);
  } // if

  return (ret);
} // set()


/**
 * @brief Activate the DisplaySH1106Element and register a Display Adapter to
 * LCD in the board.
 */
void DisplaySH1106Element::start()
{
  LOGGER_ETRACE("start()");
  DisplayAdapter *d;

  // reset of the display is available on GPIO
  if (_resetpin >= 0) {
    pinMode(_resetpin, OUTPUT);
    digitalWrite(_resetpin, LOW); // turn low to reset OLED
    delay(50);
    digitalWrite(_resetpin, HIGH); // while OLED is running, must set high
  } // if

  d = (DisplayAdapter *)(new DisplayAdapterSH1106(_address, _height));

  bool success = d->init(_board);
  if (success) {
    _board->display = d;
    Element::start();

  } else {
    LOGGER_EERR("no display found.");
    delete d;
  } // if
} // start()

// End
