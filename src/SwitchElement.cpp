/**
 * @file SwitchElement.cpp
 * @brief Element switching a digital signal HIGH LOW based on actions.
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license,
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino.
 *
 * Changelog: see SwitchElement.h
 */

#include "SwitchElement.h"
#include <Board.h>
#include <ElementRegistry.h>

/**
 * @brief static factory function to create a new SwitchElement.
 * @return SwitchElement* as Element* created element
 */
Element *SwitchElement::create()
{
  return (new SwitchElement());
} // create()


void SwitchElement::_newValue(bool val) {
  if (val != _value) {
    if (val) {
      _board->dispatch(_highAction);
      _board->dispatch(_valueAction, "1");

    } else {
      _board->dispatch(_lowAction);
      _board->dispatch(_valueAction, "0");
    } // if
    _value = val;
  } // if
} // _newValue()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool SwitchElement::set(const char *name, const char *value)
{
  LOGGER_ETRACE("set(%s:%s)", name, value);
  bool ret = true;

  if (_stricmp(name, "value") == 0) {
    _newValue(_atob(value));

  } else if (_stricmp(name, "toggle") == 0) {
    _newValue(! _value);

  } else if (_stricmp(name, "onhigh") == 0) {
    _highAction = value;

  } else if (_stricmp(name, "onlow") == 0) {
    _lowAction = value;

  } else if (_stricmp(name, "onvalue") == 0) {
    _valueAction = value;

  } else {
    ret = Element::set(name, value);
  } // if
  return (ret);
} // set()


void SwitchElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
  callback("value", String(_value).c_str());
} // pushState()

// End
