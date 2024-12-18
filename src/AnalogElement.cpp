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
#include <HomeDing.h>

#include <AnalogElement.h>

// enable TRACE for sending detailed output from this Element
#define TRACE(...)  // LOGGER_ETRACE(__VA_ARGS__)

/**
 * @brief static factory function to create a new AnalogElement.
 * @return AnalogElement* as Element* created element
 */
Element *AnalogElement::create() {
  return (new AnalogElement());
}  // create()


int AnalogElement::map(int value) {
  int divisor = _inMax - _inMin;
  int out = 0;  // better than exceptions !
  if (divisor != 0) {
    out = ((value - _inMin) * (_outMax - _outMin) / divisor) + _outMin;
  }
  if (_constrain) {
    if (_outMin < _outMax)
      out = constrain(out, _outMin, _outMax);
    else
      out = constrain(out, _outMax, _outMin);
  }
  TRACE("map(%d) %d", value, out);
  return (out);
}


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool AnalogElement::set(const char *name, const char *value) {
  bool ret = true;

  if (SensorElement::set(name, value)) {
    // done.

  } else if (name == HomeDing::Actions::Reference) {
    _reference = _atoi(value);

  } else if (_stricmp(name, "hysteresis") == 0) {
    _hysteresis = _atoi(value);

  } else if (!active) {
    // these properties can be used for configuration only.

    if (name == HomeDing::Actions::Pin) {
      _pin = _atopin(value);

      // } else if (_stricmp(name, "resolution") == 0) {
      // _resolution = _atoi(value);

    } else if (_stricmp(name, "mapInMin") == 0) {
      _inMin = _atoi(value);

    } else if (_stricmp(name, "mapInMax") == 0) {
      _inMax = _atoi(value);

    } else if (_stricmp(name, "mapOutMin") == 0) {
      _outMin = _atoi(value);

    } else if (_stricmp(name, "mapOutMax") == 0) {
      _outMax = _atoi(value);

    } else if (_stricmp(name, "constrain") == 0) {
      _constrain = _atob(value);

    } else if (name == HomeDing::Actions::OnValue) {
      _actions[0] = value;

    } else if (_stricmp(name, "onreference") == 0) {
      _actions[1] = value;

    } else if (name == HomeDing::Actions::OnHigh) {
      _highAction = value;

    } else if (name == HomeDing::Actions::OnLow) {
      _lowAction = value;

    } else {
      ret = false;
    }  // if
  } else {
    ret = false;
  }  // if
  return (ret);
}  // set()


/**
 * @brief Activate the AnalogElement.
 */
void AnalogElement::start() {
  _lastReference = -1;
  _lastValue = -1 - _hysteresis;  // use first value in any case

  // use mapping when reasonable factors are given
  _useMap = ((_inMin != _inMax) && (_outMin != _outMax));

  TRACE("pin=%d", _pin);
  TRACE("useMap=%d", _useMap);
  TRACE("map=%d...%d %d...%d", _inMin, _inMax, _outMin, _outMax);
  TRACE("constrain=%d", _constrain);

  SensorElement::start();

#if defined(ESP32)
  // set the resolution to 12 bits (0-4096)
  analogReadResolution(12);
#endif

  _valuesCount = 2;
  _stateKeys = "value,reference";
}  // start()


bool AnalogElement::getProbe(String &values) {
  int rawValue = analogRead(_pin);
  int value;

  TRACE("read(%d) %d", _pin, rawValue);

  if (_useMap) {
    value = map(rawValue);
  } else {
    value = rawValue;
  }

  if ((value >= _lastValue + _hysteresis) || (value <= _lastValue - _hysteresis)) {
    _lastValue = value;
    values = String(value) + ',' + (value < _reference ? "0" : "1");
  }  // if

  return (true);  // always simulate data is fine
}  // getProbe()


void AnalogElement::sendData(String &values) {
  SensorElement::sendData(values);

  int r = (values.endsWith(",1") ? 1 : 0);
  if (r != _lastReference) {
    HomeDing::Actions::push(r ? _highAction : _lowAction, r);
    _lastReference = r;
  }  // if
}  // sendData()

// End
