/**
 * @file AndElement.cpp
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
 * Changelog:see AndElement.h
 */

// http://nodeding/$board/and/a?value02=0
// http://nodeding/$board/and/a?value02=1

#include <Arduino.h>
#include <Board.h>
#include <Element.h>

#include "AndElement.h"
#include <ElementRegistry.h>

/* ===== Define local constants and often used strings ===== */

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new AndElement
 * @return AndElement* created element
 */
Element *AndElement::create()
{
  return (new AndElement());
} // create()


/* ===== Element functions ===== */

/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool AndElement::set(const char *name, const char *value)
{
  LOGGER_ETRACE("set(%s, %s)", name, value);
  bool ret = true;

  if (strstr(name, "value") == name) {
    // name starts with "value"
    int indx = _atoi(name + 5);
    // LOGGER_EINFO("vx(%s, %d)", name, indx);
    if ((indx > 0) && (indx <= AndElementInputs))
      _value[indx - 1] = _atob(value);

    // LOGGER_EINFO("data %d, %d", _value[0], _value[1]);

  } else if (_stricmp(name, "invert") == 0) {
    _invert = _atob(value);

  } else if (_stricmp(name, "onValue") == 0) {
    _valueAction = value;
  } else {
    ret = Element::set(name, value);
  } // if

  return (ret);
} // set()


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void AndElement::loop()
{
  // do something
  bool out = (_value[0] && _value[1]);
  if (_invert) {
    out = !out;
  }

  if (out != _outValue) {
    // LOGGER_EINFO("data %d, %d", _value[0], _value[1]);
    // LOGGER_EINFO("new value=%d", out);
    _board->dispatch(_valueAction, out);
    _outValue = out;
  }
} // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void AndElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
  callback("value", String(_outValue).c_str());
} // pushState()


/* ===== Register the Element ===== */

// As long as the Element is project specific or is a element always used
// the registration is placed here without using a register #define.

// When transferred to the HomeDing library a #define like the
// HOMEDING_INCLUDE_My should be used to allow the sketch to select the
// available Elements. See <HomeDing.h> the move these lines to AndElement.h:

// #ifdef HOMEDING_REGISTER
// Register the AndElement onto the ElementRegistry.
bool AndElement::registered =
    ElementRegistry::registerElement("and", AndElement::create);
// #endif

// End
