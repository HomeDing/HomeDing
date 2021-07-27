/**
 * @file LogicElement.cpp
 * @brief Logical Base Element that collects input values and settings.
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
 * Changelog:see LogicElement.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <LogicElement.h>


/* ===== Element functions ===== */

void LogicElement::start() {
  Element::start();
  _needRecalc = true;
}


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool LogicElement::set(const char *name, const char *value) {
  bool ret = true;
  // LOGGER_EINFO("set %s = %s", name, value);

  if (strstr(name, "value[") == name) {
    int indx = _atoi(name + 6);

    if ((indx >= 0) && (indx < MAX_INPUTS)) {
      if (_inputs < indx + 1) _inputs = indx + 1;
      _value[indx] = _atob(value);
      _needRecalc = true;
    }

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
 * @brief push the current value of all properties to the callback.
 */
void LogicElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback) {
  Element::pushState(callback);
  callback(PROP_VALUE, _printBoolean(_outValue));
} // pushState()

// End
