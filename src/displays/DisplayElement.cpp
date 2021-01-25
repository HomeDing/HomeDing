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

/* ===== Private functions ===== */

void DisplayElement::_reset()
{
  // reset
  if (_resetpin >= 0) {
    pinMode(_resetpin, OUTPUT);
    digitalWrite(_resetpin, LOW); // turn low to reset OLED
    delay(250);
    digitalWrite(_resetpin, HIGH); // while OLED is running, must set high
    delay(250);
  } // if
}

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

  } else if (_stricmp(name, "brightness") == 0) {
    _brightness = _atoi(value);
    if (active) {
      DisplayAdapter *d = _board->display;
      d->setBrightness(_brightness);
    }

  } else if (_stricmp(name, "show") == 0) {
    _page = _atoi(value); // not used yet
    _reset();

    DisplayAdapter *d = _board->display;
    d->init(_board);

    // redraw all display elements
    _board->forEach("", [this](Element *e) {
      if (_stristartswith(e->id, "display"))
        e->set("redraw", "1");
    });

  } else if (_stricmp(name, "clear") == 0) {
    DisplayAdapter *d = _board->display;
    d->init(_board);
    _page = 0; // always empty

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
  _reset();
} // start()

/**
 * @brief push the current value of all properties to the callback.
 */
void DisplayElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
  callback("brightness", String(_brightness).c_str());
  callback("page", String(_page).c_str());
} // pushState()

// End
