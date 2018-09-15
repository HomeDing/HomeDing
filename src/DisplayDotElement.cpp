/**
 * @file DisplayDotElement.cpp
 * @brief Output Element for controlling a binary output on a display.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license,
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino.
 *
 * Changelog, see DisplayDotElement.h.
 */

#include "DisplayDotElement.h"

#define LOGGER_MODULE "DisplayDot"
#include "core/Logger.h"

/**
 * @brief static factory function to create a new DisplayDotElement.
 * @return DisplayDotElement* as Element* created element
 */
Element *DisplayDotElement::create()
{
  LOGGER_TRACE("create()");
  return (new DisplayDotElement());
} // create()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool DisplayDotElement::set(const char *name, const char *value)
{
  LOGGER_TRACE("set(%s:%s)", name, value);
  bool ret = true;

  if (_stricmp(name, "x") == 0) {
    _x = atoi(value);

  } else if (_stricmp(name, "y") == 0) {
    _y = atoi(value);

  } else if (!active) {
    // no actions.
    LOGGER_TRACE(" not active: %d.", _display);


  } else if (_stricmp(name, "clear") == 0) {
    _display->clear();

  } else if ((_stricmp(name, "show") == 0) || (_stricmp(name, "value") == 0)) {
    _display->clear(_x, _y, _w, _h);

    _value = value;
    _display->drawDot(_x, _y, _h, _atob(value));

  } else {
    ret = Element::set(name, value);
  } // if

  return (ret);
} // set()


/**
 * @brief Activate the DisplayDotElement.
 */
void DisplayDotElement::start()
{
  DisplayAdapter *d = (DisplayAdapter *)(_board->display);

  if (d == NULL) {
    LOGGER_ERR("no display defined");

  } else {
    _display = d;
    Element::start();
    if (active) {
      set("show", ""); // display the default UI for some types
    } // if
  } // if
} // start()


/**
 * @brief push the current value of all properties to the callback.
 */
void DisplayDotElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
  callback("value", _value.c_str());
} // pushState()

// End
