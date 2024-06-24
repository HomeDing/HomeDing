// -----
// PWMOutElement.cpp - Library for creating an PWM outgoing signal.
//
// Copyright (c) by Matthias Hertel, https://www.mathertel.de.
//
// This work is licensed under a BSD style license.
// See https://www.mathertel.de/License.aspx.
// More information on https://www.mathertel.de/Arduino
// -----
// Changelog: see PWMOutElement.h
// -----

#include <Arduino.h>
#include <HomeDing.h>

#include <PWMOutElement.h>

/**
 * @brief static factory function to create a new PWMOutElement.
 * @return PWMOutElement* as Element* created element
 */
Element *PWMOutElement::create() {
  PWMOutElement *e = new PWMOutElement();
  e->category = CATEGORY::Standard;  // no polling
  return (e);
}  // create()


bool PWMOutElement::set(const char *name, const char *value) {
  bool ret = true;

  if (name == HomeDing::Action::Value) {
    _setValue(_atoi(value));

  } else if (name == HomeDing::Action::Pin) {
    _pin = _atopin(value);

  } else if (_stricmp(name, "invert") == 0) {
    _inverse = _atob(value);

  } else if (_stricmp(name, "range") == 0) {
    _range = _atoi(value);

  } else {
    ret = Element::set(name, value);
  }  // if
  return (ret);
}  // set()


/**
 * @brief Activate the PWMOutElement.
 */
void PWMOutElement::start() {
  if (_pin < 0) {
    LOGGER_EERR("no pin");

  } else {
    // enable output and stay off
    Element::start();

#if defined(ESP8266)
    pinMode(_pin, OUTPUT);
    analogWriteRange(_range);

#elif (defined(ESP32))

#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
    pinMode(_pin, OUTPUT);
    analogWriteFrequency(_pin, 8000);
    analogWriteResolution(_pin, 8);
#else
    _channel = _board->nextLedChannel++;
    ledcSetup(_channel, 8000, 8);
    ledcAttachPin(_pin, _channel);
#endif

#endif

    _setValue(_value);

  }  // if
}  // start()


void PWMOutElement::pushState(
  std::function<void(const char *pName, const char *eValue)> callback) {
  Element::pushState(callback);
  callback("value", _printInteger(_value));
}  // pushState()


/**
 * @brief set the physical level based on _inverse
 * @param logicalHigh
 */
void PWMOutElement::_setValue(int newValue) {
  // TRACE("setValue(%d)", newValue);
  _value = newValue;

  if (active) {
    int v = (_inverse) ? _range - _value : _value;
#if defined(ESP8266)
    analogWrite(_pin, v);
#elif (defined(ESP32))
    analogWrite(_pin, v);
#endif

  }  // if
}  // _setValue


// End
