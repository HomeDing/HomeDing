/**
 * @file AnalogElement.cpp
 * @brief Analog Input Element for the HomeDing Library.
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license,
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino.
 *
 * Changelog: see AnalogElement.h
 */

#include "AnalogElement.h"
#include <Board.h>
#include <ElementRegistry.h>

/**
 * @brief static factory function to create a new AnalogElement.
 * @return AnalogElement* as Element* created element
 */
Element *AnalogElement::create()
{
  return (new AnalogElement());
} // create()


AnalogElement::AnalogElement()
{
  // default values;
  _pin = A0; // always.

  _readTime = 1; // read from sensor once every second
  _reference = 500;
  _hysteresis = 10;

  // _resendTime = 0; // Not enabled resending probes.
}


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool AnalogElement::set(const char *name, const char *value)
{
  LOGGER_ETRACE("set(%s:%s)", name, value);
  bool ret = true;

  if (_stricmp(name, "readtime") == 0) {
    _readTime = _atotime(value);

  } else if (_stricmp(name, "hysteresis") == 0) {
    _hysteresis = _atoi(value);

  } else if (_stricmp(name, "reference") == 0) {
    _reference = _atoi(value);

  } else if (_stricmp(name, "onreference") == 0) {
    _referenceAction = value;

  } else if (_stricmp(name, "onvalue") == 0) {
    _valueAction = value;

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
  unsigned int now = (millis() / 1000);
  _nextRead = now + 2;
  // _nextResend = now + _resendTime;
  _lastReference = -1;

  Element::start();
} // start()


/**
 * @brief check the state of the input.
 */
void AnalogElement::loop()
{
  unsigned int now = (millis() / 1000);

  if (_nextRead <= now) {
    int v = analogRead(_pin);
    // LOGGER_ETRACE("read(%d)", v);

    if ((v >= _value + _hysteresis) || (v <= _value - _hysteresis)) {
      _value = v;
      // LOGGER_ETRACE("dispatch(%d)", v);

      if (_valueAction.length() > 0) {
        // send value action
        char tmp[10];
        itoa(v, tmp, 10);
        _board->dispatch(_valueAction, tmp);
      } // if

      if (_referenceAction.length() > 0) {
      // compare against reference and send reference action
        int r = (v < _reference ? 0 : 1);
        if (r != _lastReference) {
          _board->dispatch(_referenceAction, (v < _reference ? "0" : "1"));
          _lastReference = r;
        } // if
      } // if
    } // if

    _nextRead = now + _readTime;
  }
} // loop()


void AnalogElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
  callback("value", String(_value).c_str());
  callback("reference", String(_lastReference).c_str());
} // pushState()

// End
