/**
 * @file ValueElement.cpp
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

  if (newValue < _minRange)
    newValue = _minRange;

  if (newValue > _maxRange)
    newValue = _maxRange;

  if (forceAction || ((active) && (_value != newValue))) {
    _board->dispatch(_valueAction, newValue);
    ret = true;
  }
  _value = newValue;
  return (ret);
} // _setValue()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool ValueElement::set(const char *name, const char *value)
{
  bool ret = true;

  if (_stricmp(name, PROP_VALUE) == 0) {
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

  // } else if (_stricmp(name, "onchange") == 0) { // deprecated: use onValue
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
  _setValue(_value, true);
} // start()


/**
 * @brief push the current value of all properties to the callback.
 */
void ValueElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
  callback(PROP_VALUE, String(_value).c_str());
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
