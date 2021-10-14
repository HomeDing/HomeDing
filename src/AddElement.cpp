/**
 * @file AddElement.cpp
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
 * Changelog:see AddElement.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <AddElement.h>


/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new AndElement
 * @return AddElement* created element
 */
Element *AddElement::create() {
  return (new AddElement());
} // create()


/* ===== Element functions ===== */

void AddElement::start() {
  Element::start();
  _needRecalc = true;
}


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool AddElement::set(const char *name, const char *value) {
  bool ret = true;
  // LOGGER_EINFO("set %s = %s", name, value);

  if (strstr(name, "value[") == name) {
    int indx = _atoi(name + 6);

    if ((indx >= 0) && (indx < MAX_INPUTS)) {
      if (_inputs < indx + 1) _inputs = indx + 1;
      _inValue[indx] = _atoi(value);
      _needRecalc = true;
    }

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
void AddElement::loop() {
  if (_needRecalc) {
    int newValue = 0;

    for (int n = 0; n < _inputs; n++) {
      newValue += _inValue[n];
    }

    if (newValue != _value) {
      _board->dispatch(_valueAction, newValue);
      _value = newValue;
    }

    _needRecalc = false;
  } // if
  Element::loop();
} // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void AddElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback) {
  Element::pushState(callback);
  callback(PROP_VALUE, _printInteger(_value));
} // pushState()

// End
