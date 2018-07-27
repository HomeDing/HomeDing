// -----
// Display.cpp - Library for creating a digital outgoing signal.
//
// Copyright (c) by Matthias Hertel, https://www.mathertel.de.
//
// This work is licensed under a BSD style license.
// See https://www.mathertel.de/License.aspx.
// More information on https://www.mathertel.de/Arduino
// -----
// Changelog: see Display.h
// -----

// https://stackoverflow.com/questions/18806141/move-object-creation-to-setup-function-of-arduino

#include "DisplayElement.h"
#include "ElementRegistry.h"

#define LOGGER_MODULE "Display"
#include "Logger.h"

/**
 * @brief static factory function to create a new DisplayElement.
 * @return DisplayElement* as Element* created element
 */
Element *DisplayElement::create()
{
  LOGGER_TRACE("create()");
  return (new DisplayElement());
} // create()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool DisplayElement::set(const char *name, const char *value)
{
  LOGGER_TRACE("set(%s:%s)", name, value);
  bool ret = true;

  if (_stricmp(name, "prefix") == 0) {
    if (strlen(value) < sizeof(_prefix))
      strcpy(_prefix, value);

  } else if (_stricmp(name, "postfix") == 0) {
    if (strlen(value) < sizeof(_postfix))
      strcpy(_postfix, value);

  } else if (_stricmp(name, "type") == 0) {
    if (_stricmp(value, "text") == 0) {
      _type = DISPLAY_TYPE_TEXT;
      _w = 100;
      _h = 10;
    } else if (_stricmp(value, "dot") == 0) {
      _type = DISPLAY_TYPE_DOT;
      _w = 11;
      _h = 11;
    } // if

  } else if (_stricmp(name, "x") == 0) {
    _x = atoi(value);

  } else if (_stricmp(name, "y") == 0) {
    _y = atoi(value);

  } else if (_stricmp(name, "fontsize") == 0) {
    int s = atoi(value);
    if ((s == 10) || (s == 16) || (s == 24)) {
      _h = s;
    } // if


  } else if (! active) {
    // no actions.
    LOGGER_TRACE(" not active: %d.", _display);


  } else if (_stricmp(name, "clear") == 0) {
    _display->clear();

  } else if ((_stricmp(name, "show") == 0) || (_stricmp(name, "value") == 0)) {
    _display->clear(_x, _y, _w, _h);

    if (_type == DISPLAY_TYPE_TEXT) {
      _value = value;

      String msg(_prefix);
      msg.concat(value);
      msg.concat(_postfix);
      _display->drawText(_x, _y, _h, msg);

    } else if (_type == DISPLAY_TYPE_DOT) {
      _value = value;
      bool b = _atob(value);
      _display->drawDot(_x, _y, _h, b);
    } // if

  } else {
    ret = Element::set(name, value);
  } // if

  return (ret);
} // set()


/**
 * @brief Activate the DisplayElement.
 */
void DisplayElement::start()
{
  DisplayAdapter *d = (DisplayAdapter *)(_board->display);

  if (d == NULL) {
    LOGGER_ERR("no display defined");

  } else {
    _display = d;
    Element::start();
  } // if

  if (active) {
    // display the default UI for some types
    if (_type == DISPLAY_TYPE_DOT)
      set("show", "");
  } // if
} // start()


/**
 * @brief push the current value of all properties to the callback.
 */
void DisplayElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
  callback("value", _value.c_str());
} // pushState()


// Register the DisplayElement onto the ElementRegistry.
bool DisplayElement::registered =
    ElementRegistry::registerElement("display", DisplayElement::create);

// End
