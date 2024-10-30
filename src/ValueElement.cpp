/**
 * @file ValueElement.cpp
 * @brief The ValueElement combines receiving modifying actions for an value and sending actions on changing the value.
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
 * Changelog:see ValueElement.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <ValueElement.h>

/**
 * @brief static factory function to create a new ValueElement
 * @return ValueElement* created element
 */
Element *ValueElement::create() {
  return (new ValueElement());
}  // create()


// set a new int-type value, maybe adjust to range
bool ValueElement::_setValue(int newValue, bool forceAction) {
  bool ret = false;
  _isValid = true;

  newValue = constrain(newValue, _minRange, _maxRange);

  if (forceAction || ((active) && (_value != newValue))) {
    HomeDing::Actions::push(_valueAction, newValue);
    ret = true;
  }  // if

  if (_value != newValue) {
    _value = newValue;
    saveState(HomeDing::Actions::Value, String(_value));
  }

  return (ret);
}  // _setValue()


// set a new string-type value
bool ValueElement::_setValue(const char *newValue, bool forceAction) {
  bool ret = false;
  _isValid = true;

  if (forceAction || ((active) && (_valueString != newValue))) {
    HomeDing::Actions::push(_valueAction, newValue);
    ret = true;
  }

  if (_valueString != newValue) {
    _valueString = newValue;
    saveState(HomeDing::Actions::Value, _valueString);
  }

  return (ret);
}  // _setValue()


int ValueElement::_getValueInt() {
  return (_value);
}


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool ValueElement::set(const char *name, const char *value) {
  bool ret = true;

  if (Element::set(name, value)) {
    // done

  } else if (name == HomeDing::Actions::Value) {
    if (_isStringType)
      _setValue(value);
    else
      _setValue(_atoi(value));

  } else if (name == HomeDing::Actions::Up) {
    _setValue(_value + _atoi(value) * _step);

  } else if (name == HomeDing::Actions::Down) {
    _setValue(_value - _atoi(value) * _step);

  } else if (name == HomeDing::Actions::Min) {
    _minRange = _atoi(value);

  } else if (name == HomeDing::Actions::Max) {
    _maxRange = _atoi(value);

  } else if (name == HomeDing::Actions::Step) {
    _step = _atoi(value);

  } else if (name == HomeDing::Actions::Label) {
    _label = value;

  } else if (name == HomeDing::Actions::Type) {
    if (_stricmp(value, "string") == 0)
      _isStringType = true;

  } else if (name == HomeDing::Actions::OnValue) {
    _valueAction = value;

  } else {
    ret = false;
  }  // if

  return (ret);
}  // set()


void ValueElement::start() {
  Element::start();

  // Generate label from id if not given
  if (_label.length() == 0) {
    _label = strchr(id, ELEM_ID_SEPARATOR) + 1;
  }

  // send out the actual defined value
  if (_isValid) {
    if (_isStringType) {
      _setValue(_valueString.c_str(), true);

    } else {
      _setValue(_value, true);
    }
  }  // if
}  // start()


/**
 * @brief push the current value of all properties to the callback.
 */
void ValueElement::pushState(
  std::function<void(const char *pName, const char *eValue)> callback) {
  Element::pushState(callback);
  callback(HomeDing::Actions::Value, (_isStringType ? _valueString : String(_value)).c_str());
}  // pushState()

/** return actual value */
int ValueElement::getValue() {
  return (_value);
}

/** return actual value */
const char *ValueElement::getLabel() {
  return (_label.c_str());
}


// End
