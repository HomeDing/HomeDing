/**
 * @file CalcElement.cpp
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
 * Changelog:see CalcElement.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <CalcElement.h>


/* ===== Element functions ===== */

void CalcElement::start() {
  Element::start();
  _needRecalc = true;
}


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool CalcElement::set(const char *name, const char *value) {
  bool ret = true;
  // LOGGER_EINFO("set %s = %s", name, value);

  if (strstr(name, "value[") == name) {
    int indx = _atoi(name + 6);

    if ((indx >= 0) && (indx < CALCELEMENT_MAX_INPUTS)) {
      if (_inputs < indx + 1) _inputs = indx + 1;

      if (_type == DATATYPE::BOOLEAN) {
        _inStringValues[indx] = _atob(value);
      } else if (_type == DATATYPE::INTEGER) {
        _inStringValues[indx] = _atoi(value);
      } else if (_type == DATATYPE::FLOAT) {
        _inStringValues[indx] = strtod(value, nullptr);
      } else {
        _inStringValues[indx] = value;
      }
      _needRecalc = true;
    }

  } else if (_stricmp(name, "invert") == 0) {
    // the invert property is used for AND and OR element.
    _invert = _atob(value);

  } else if (_stricmp(name, ACTION_ONVALUE) == 0) {
    _valueAction = value;
  } else {
    ret = Element::set(name, value);
  }  // if

  return (ret);
}  // set()


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void CalcElement::loop() {
  if (_needRecalc) {
    String oldValue = _value;

    _calc();
    if (_value != oldValue) {
      _board->dispatch(_valueAction, _value);
    }
    _needRecalc = false;
  }  // if
  Element::loop();
}  // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void CalcElement::pushState(
  std::function<void(const char *pName, const char *eValue)> callback) {
  Element::pushState(callback);
  callback(PROP_VALUE, _value.c_str());
}  // pushState()


/**
 * @brief function for calculating from input to output values.
 */
void CalcElement::_calc(){};


// End
