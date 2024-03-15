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

#if ! defined(TRACE)
#define TRACE(...) // LOGGER_ETRACE(__VA_ARGS__)
#endif

#define STATE_OFF 0   // just got power on
#define STATE_WAIT 1  // initialized, warmup or wait time beween probes
#define STATE_READ 2  // Reading a value
#define STATE_SEND 3  // Sending Data

SensorElement::SensorElement() {}

/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool SensorElement::set(const char *name, const char *value) {
  bool ret = true;

  if (Element::set(name, value)) {
    // done.
  } else if (_stricmp(name, "readtime") == 0) {
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
  return (ret);
}  // set()


/**
 * @brief Activate the SensorElement.
 */
void SensorElement::start() {
  TRACE("start()");
  unsigned int now = millis();
  Element::start();

  // _sensorWorkedOnce = false;
  _state = STATE_WAIT;
  _lastRead = now + _warmupTime - _readTime;  // now + some seconds. allowing the sensor to get values.
  _nextSend = 0;
}  // start()


/**
 * @brief Dectivate the SensorElement.
 * This method is called when the sensor stopped working.
 * It reactivates the element when specified by restart property.
 */
void SensorElement::term() {
  TRACE("term()");
  Element::term();
  if (_state == STATE_READ && _sensorWorkedOnce && _restart) {
    // term() was initiated by sensor element and retry is configured.
    this->start();
  }
}  // term()


/**
 * @brief read or send sensor data.
 * This method will not be called when the element is not active.
 */
void SensorElement::loop() {
  unsigned long now = _board->nowMillis;
  String value;

  if (_waitStart) {
    if ((now - _waitStart) >= _waitDuration) {
      _waitStart = 0;  // stop waiting.
    }

  } else if (_state == STATE_WAIT) {
    if ((now - _lastRead) < _readTime) {
      // just wait on...

    } else {
      _state = STATE_READ;
      _startTime = now;
    }

  } else if (_state == STATE_READ) {
    // time to get sensor data, repeat until returning true
    // TRACE("reading...");

    if (getProbe(value)) {
      if (value.length() > 0) {
        // it's a valid value from the sensor
        _sensorWorkedOnce = true;
        if (!value.equals(_lastValues)) {
          _lastValues = value;
          _nextSend = now;  // enforce sending now
          _state = STATE_SEND;
        }  // if

      } else if (_nextSend && (_nextSend < now)) {
        _state = STATE_SEND;

      } else {
        _lastRead = now;
        _state = STATE_WAIT;
      }  // if

    }  // if

  } else if (_state == STATE_SEND) {
    // time to send sensor data
    // TRACE("sending...");
    if (!_lastValues.isEmpty())
      sendData(_lastValues);

    _lastRead = now;
    _nextSend = (_resendTime ? now + _resendTime : 0);
    _state = STATE_WAIT;
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


// ===== protected functions =====

void SensorElement::setWait(unsigned long waitMilliseconds) {
  TRACE("setWait(%d)", waitMilliseconds);
  _waitStart = millis();
  _waitDuration = waitMilliseconds;
}

bool SensorElement::getProbe(String & /*values */) {
  return (true);  // always simulate data is fine
}  // getProbe()


void SensorElement::sendData(String &values) {
  TRACE("sendData()");
  for (int n = 0; n < _valuesCount; n++) {
    _board->dispatchItem(_actions[n], values, n);
  }
}  // sendData()


// End
