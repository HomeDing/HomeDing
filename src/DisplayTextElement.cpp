/**
 * @file DisplayTextElement.cpp
 * @brief Output Element for controlling a text output on a display.
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
 * Changelog, see DisplayTextElement.h.
 */

#include "DisplayTextElement.h"

#define LOGGER_MODULE "DisplayText"
#include "core/Logger.h"

/**
 * @brief static factory function to create a new DisplayTextElement.
 * @return DisplayTextElement* as Element* created element
 */
Element *DisplayTextElement::create()
{
  LOGGER_TRACE("create()");
  return (new DisplayTextElement());
} // create()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool DisplayTextElement::set(const char *name, const char *value)
{
  LOGGER_TRACE("set(%s:%s)", name, value);
  bool ret = true;

  if (_stricmp(name, "prefix") == 0) {
    if (strlen(value) < sizeof(_prefix))
      strcpy(_prefix, value);

  } else if (_stricmp(name, "postfix") == 0) {
    if (strlen(value) < sizeof(_postfix))
      strcpy(_postfix, value);

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

    _value = value;

    String msg(_prefix);
    msg.concat(value);
    msg.concat(_postfix);
    _w = _display->drawText(_x, _y, _h, msg);

  } else {
    ret = Element::set(name, value);
  } // if

  return (ret);
} // set()


/**
 * @brief Activate the DisplayTextElement.
 */
void DisplayTextElement::start()
{
  DisplayAdapter *d = (DisplayAdapter *)(_board->display);

  if (d == NULL) {
    LOGGER_ERR("no display defined");

  } else {
    _display = d;
    Element::start();
  } // if
} // start()


/**
 * @brief push the current value of all properties to the callback.
 */
void DisplayTextElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
  callback("value", _value.c_str());
} // pushState()

// End
