/**
 * @file AndElement.cpp
 * @brief Logical Element that combines boolean input values using the AND and optional NOT operator and sends actions.
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

#include <Arduino.h>
#include <HomeDing.h>

#include <AndElement.h>

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
  bool ret = true;

  if (strstr(name, PROP_VALUE) == name) {
    // name starts with "value"
    int indx = _atoi(name + 5);
    // LOGGER_EINFO("vx(%s, %d)", name, indx);
    if ((indx > 0) && (indx <= AndElementInputs))
      _value[indx - 1] = _atob(value);

    // LOGGER_EINFO("data %d, %d", _value[0], _value[1]);

  } else if (_stricmp(name, "invert") == 0) {
    _invert = _atob(value);

  } else if (_stricmp(name, ACTION_ONVALUE) == 0) {
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
  callback(PROP_VALUE, _printBoolean(_outValue));
} // pushState()

// End
