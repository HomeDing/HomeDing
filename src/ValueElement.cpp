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
#include <Board.h>
#include <Element.h>

#include <ValueElement.h>

/**
 * @brief static factory function to create a new ValueElement
 * @return ValueElement* created element
 */
Element *ValueElement::create()
{
  return (new ValueElement());
} // create()


// set a new value, maybe adjust to range
bool ValueElement::_setValue(int newValue, bool forceAction)
{
  bool ret = false;
  _isValid = true;
  
  newValue = constrain(newValue, _minRange, _maxRange);

  if (forceAction || ((active) && (_value != newValue))) {
    _board->dispatch(_valueAction, newValue);
    ret = true;
  } // if
  
  _value = newValue;
  return (ret);
} // _setValue()


// set a new value, maybe adjust to range
bool ValueElement::_setValue(const char *newValue, bool forceAction)
{
  bool ret = false;
  _isValid = true;

  if (forceAction || ((active) && (_valueString != newValue))) {
    _board->dispatch(_valueAction, newValue);
    ret = true;
  }
  _valueString = newValue;
  return (ret);
} // _setValue()


int ValueElement::_getValueInt()
{
  return (_value);
}


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool ValueElement::set(const char *name, const char *value)
{
  bool ret = true;

  if (_stricmp(name, PROP_VALUE) == 0) {
    if (_isStringType)
      _setValue(value);
    else
      _setValue(_atoi(value));

  } else if (_stricmp(name, "up") == 0) {
    _setValue(_value + _atoi(value) * _step);

  } else if (_stricmp(name, "down") == 0) {
    _setValue(_value - _atoi(value) * _step);

  } else if (_stricmp(name, "min") == 0) {
    _minRange = _atoi(value);

  } else if (_stricmp(name, "max") == 0) {
    _maxRange = _atoi(value);

  } else if (_stricmp(name, "step") == 0) {
    _step = _atoi(value);

  } else if (_stricmp(name, "label") == 0) {
    _label = value;

  } else if (_stricmp(name, "type") == 0) {
    if (_stricmp(value, "string") == 0)
      _isStringType = true;

  } else if (_stricmp(name, ACTION_ONVALUE) == 0) {
    _valueAction = value;

  } else {
    ret = Element::set(name, value);
  } // if

  return (ret);
} // set()


void ValueElement::start()
{
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
  } // if
} // start()


/**
 * @brief push the current value of all properties to the callback.
 */
void ValueElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
  callback(PROP_VALUE, (_isStringType ? _valueString : String(_value)).c_str());
} // pushState()

/** return actual value */
int ValueElement::getValue()
{
  return (_value);
}

/** return actual value */
const char *ValueElement::getLabel()
{
  return (_label.c_str());
}


// End
