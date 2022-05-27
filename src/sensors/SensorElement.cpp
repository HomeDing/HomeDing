/**
 * @file SensorElement.cpp
 *
 * @brief This Input Element acts as the base class for some sensor elements that need collecting sensor data on a regular basis.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license,
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino.
 *
 * Changelog: see SensorElement.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <sensors/SensorElement.h>

// setup default timings
SensorElement::SensorElement() {
}  // SensorElement()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool SensorElement::set(const char *name, const char *value) {
  bool ret = Element::set(name, value);

  if (!ret) {
    ret = true;
    if (_stricmp(name, "readtime") == 0) {
      _readTime = _scanDuration(value);

    } else if (_stricmp(name, "resendtime") == 0) {
      _resendTime = _scanDuration(value);

    } else if (_stricmp(name, "warmuptime") == 0) {
      _warmupTime = _scanDuration(value);

    } else if (_stricmp(name, "restart") == 0) {
      _restart = _atob(value);

    } else {
      ret = false;
    }  // if
  }    // if
  return (ret);
}  // set()


/**
 * @brief Activate the SensorElement.
 */
void SensorElement::start() {
  unsigned int now = millis();
  Element::start();

  _nextRead = now + _warmupTime;  // now + some seconds. allowing the sensor to get values.
  _nextSend = 0;
}  // start()


/**
 * @brief Dectivate the SensorElement.
 * This method is called when the sensor stopped working.
 * It reactivates the element when specified by restart property.
 */
void SensorElement::term() {
  unsigned int now = millis();
  Element::term();
  if (_isReading && _sensorWorkedOnce && _restart) {
    // term() was initiated by sensor element and retry is configured.
    this->start();
    _sensorWorkedOnce = false;
  }
  _nextRead = now + _warmupTime;  // now + some seconds. allowing the sensor to get values.
  _nextSend = 0;
}  // start()


/**
 * @brief read or send sensor data.
 * This method will not be called when the element is not active.
 */
void SensorElement::loop() {
  unsigned long now = millis();
  String value;

  if (_nextRead <= now) {
    // time to get sensor data, repeat until returning true
    // TRACE("reading...");
    _isReading = true;
    bool done = getProbe(value);
    _isReading = false;

    if (done) {
      if (value.length() > 0) {
        // it's a valid value from the sensor
        _sensorWorkedOnce = true;
        if (!value.equals(_lastValues)) {
          _lastValues = value;
          _nextSend = now;  // enforce sending now
        }                   // if
      }                     // if
      _nextRead = now + _readTime;
    }  // if

  } else if (_nextSend && (_nextSend < now)) {
    // TRACE("sending...");
    if (!_lastValues.isEmpty())
      sendData(_lastValues);
    _nextSend = _resendTime ? now + _resendTime : 0;
  }  // if
}  // loop()


void SensorElement::pushState(
  std::function<void(const char *pName, const char *eValue)> callback) {
  Element::pushState(callback);
  for (int n = 0; n < _valuesCount; n++) {
    callback(
      Element::getItemValue(_stateKeys, n).c_str(),
      Element::getItemValue(_lastValues, n).c_str());
  }
}  // pushState()


// ===== private functions =====

bool SensorElement::getProbe(UNUSED String &values) {
  return (true);  // always simulate data is fine
}  // getProbe()


void SensorElement::sendData(UNUSED String &values) {
  if (_valuesCount >= 1) { _board->dispatchItem(_value00Action, values, 0); }
  if (_valuesCount >= 2) { _board->dispatchItem(_value01Action, values, 1); }
}  // sendData()


// End
