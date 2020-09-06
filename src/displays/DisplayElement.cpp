/**
 * @file DisplayElement.cpp
 * @brief Implementation of the base function for a DisplayXXXElement.
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
 * Changelog:see DisplayElement.h
 */

#include <Arduino.h>
#include <Board.h>
#include <HomeDing.h>

#include "DisplayElement.h"

// #define LOG_TRACE(...) LOGGER_TRACE(__VA_ARGS__)
#define LOG_TRACE(...)

/* ===== Element functions ===== */

/**
 * @brief Constructor of a new DisplayElement.
 */
DisplayElement::DisplayElement()
{
  startupMode = Element_StartupMode::System;
}


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool DisplayElement::set(const char *name, const char *value)
{
  bool ret = true;

  if (_stricmp(name, PROP_ADDRESS) == 0) {
    _address = _atoi(value);

  } else if (_stricmp(name, "resetpin") == 0) {
    _resetpin = _atopin(value);

  } else if (_stricmp(name, "height") == 0) {
    _height = _atoi(value);

  } else if (_stricmp(name, "width") == 0) {
    _width = _atoi(value);

  } else if (_stricmp(name, "show") == 0) {
    int page = _atoi(value); // not used yet

    // reset
    if (_resetpin >= 0) {
      pinMode(_resetpin, OUTPUT);
      digitalWrite(_resetpin, LOW); // turn low to reset OLED
      delay(50);
      digitalWrite(_resetpin, HIGH); // while OLED is running, must set high
    } // if
    DisplayAdapter *d = _board->display;
    d->init(_board);

    // redraw all display elements
    _board->forEach("", [this](Element *e) {
      LOG_TRACE("forEach %s", e->id);
      if (_stristartswith(e->id, "display"))
        e->set("redraw", "1");
    });

  } else if (_stricmp(name, "clear") == 0) {
    DisplayAdapter *d = _board->display;
    d->init(_board);

  } else {
    ret = Element::set(name, value);
  } // if

  return (ret);
} // set()


/**
 * @brief Activate the DisplayElement when a resetpin is given.
 */
void DisplayElement::start()
{
  // TRACE("start()");
  Element::start();

  // reset of the display is available on GPIO
  if (_resetpin >= 0) {
    pinMode(_resetpin, OUTPUT);
    digitalWrite(_resetpin, LOW); // turn low to reset OLED
    delay(50);
    digitalWrite(_resetpin, HIGH); // while OLED is running, must set high
  } // if
} // start()

// End
