/**
 * @file AnalogElement.cpp
 * @brief Analog Input Element for the HomeDing Library.
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD 3-Clause style license,
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino.
 *
 * Changelog: see AnalogElement.h
 */

#include <Arduino.h>
#include <Board.h>
#include <Element.h>

#include "AnalogElement.h"

#define TRACE(...) // LOGGER_ETRACE(__VA_ARGS__)

/**
 * @brief static factory function to create a new AnalogElement.
 * @return AnalogElement* as Element* created element
 */
Element *AnalogElement::create()
{
  return (new AnalogElement());
} // create()


AnalogElement::AnalogElement()
    : _pin(A0),         // always.
      _readTimeMS(100), // read from sensor 10 times a second.
      _nextReadMS(0),
      _reference(500),
      _hysteresis(10)
{
}

float AnalogElement::mapFloat(int value)
{
  return (value - _inMin) * (_outMax - _outMin) / (_inMax - _inMin) + _outMin;
}


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool AnalogElement::set(const char *name, const char *value)
{
  bool ret = true;

  if (_stricmp(name, "readtimems") == 0) {
    _readTimeMS = _atoi(value);

  } else if (_stricmp(name, "hysteresis") == 0) {
    _hysteresis = _atoi(value);

    // } else if (_stricmp(name, PROP_PIN) == 0) {
    //   _pin = _atopin(value);
    // pin is always A0

    // } else if (_stricmp(name, "resolution") == 0) {
    // _resolution = _atoi(value); ???

  } else if (_stricmp(name, "mapInMin") == 0) {
    _inMin = _atoi(value);

  } else if (_stricmp(name, "mapInMax") == 0) {
    _inMax = _atoi(value);

  } else if (_stricmp(name, "mapOutMin") == 0) {
    _outMin = _atoi(value);

  } else if (_stricmp(name, "mapOutMax") == 0) {
    _outMax = _atoi(value);

  } else if (_stricmp(name, "reference") == 0) {
    _reference = _atoi(value);

  } else if (_stricmp(name, ACTION_ONVALUE) == 0) {
    _valueAction = value;

  } else if (_stricmp(name, "onreference") == 0) {
    _referenceAction = value;

  } else if (_stricmp(name, "onhigh") == 0) {
    _highAction = value;

  } else if (_stricmp(name, "onlow") == 0) {
    _lowAction = value;

  } else {
    ret = Element::set(name, value);
  } // if
  return (ret);
} // set()


/**
 * @brief Activate the AnalogElement.
 */
void AnalogElement::start()
{
  _nextReadMS = millis() + _readTimeMS;
  _lastReference = -1;

  // use mapping when reasonable factors are given
  _useMap = ((_inMin != _inMax) && (_outMin != _outMax));

  Element::start();
} // start()

int rawValue;

/**
 * @brief check the state of the input.
 */
void AnalogElement::loop()
{
  unsigned int now = millis();

  if (_nextReadMS <= now) {
    int v = analogRead(_pin);
    rawValue = v;
    if (_useMap) {
      v = mapFloat(v);
    }
    TRACE("read(%d=>%d)", rawValue, v);

    if ((v >= _value + _hysteresis) || (v <= _value - _hysteresis)) {

      _value = v;
      if (_valueAction.length() > 0)
        _board->dispatch(_valueAction, v);

      // compare against reference and send reference actions
      int r = (v < _reference ? 0 : 1);
      if (r != _lastReference) {
        _board->dispatch(_referenceAction, r);

        if (r) {
          _board->dispatch(_highAction);
        } else {
          _board->dispatch(_lowAction);
        } // if

        _lastReference = r;
      } // if
    }   // if

    _nextReadMS = millis() + _readTimeMS;
  }
} // loop()


void AnalogElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
  callback(PROP_VALUE, String(_value).c_str());
  callback("rawvalue", String(rawValue).c_str());
  callback("reference", String(_lastReference).c_str());
} // pushState()

// End
