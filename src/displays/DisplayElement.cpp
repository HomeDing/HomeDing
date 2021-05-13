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

#define TRACE(...) // LOGGER_ETRACE(__VA_ARGS__)

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
} // _reset()


void DisplayElement::_newPage(int page)
{
  TRACE("newPage %d", page);
  DisplayAdapter *d = _board->display;
  int oldPage = d->page;

  d->page = constrain(page, 0, d->maxpage);
  _reset();
  d->init(_board);

  // redraw all display elements
  _board->forEach("display", [this](Element *e) {
    e->set("redraw", "1");
  });
  if (d->page != oldPage) {
    _board->dispatch(_onPage, d->page);
  }
} // _newPage()


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
  TRACE("set %s=%s", name, value);

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

  } else if (_stricmp(name, "page") == 0) {
    DisplayAdapter *d = _board->display;
    _newPage(*value ? _atoi(value) : d->page);

  } else if (_stricmp(name, "onpage") == 0) {
    // action with current visible page
    _onPage = value;

  } else if (_stricmp(name, "addpage") == 0) {
    DisplayAdapter *d = _board->display;
    _newPage(d->page + _atoi(value));

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
  callback("page", String(_board->display->page).c_str());
} // pushState()

// End
