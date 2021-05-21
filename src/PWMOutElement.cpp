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
Element *PWMOutElement::create()
{
  return (new PWMOutElement());
} // create()


bool PWMOutElement::set(const char *name, const char *value)
{
  bool ret = true;

  if (_stricmp(name, PROP_VALUE) == 0) {
    _setValue(_atoi(value));

  } else if (_stricmp(name, PROP_PIN) == 0) {
    _pin = _atopin(value);

  } else if (_stricmp(name, PROP_INVERSE) == 0) {
    _inverse = _atob(value);

  } else if (_stricmp(name, "range") == 0) {
    _range = _atoi(value);

  } else {
    ret = Element::set(name, value);
  } // if
  return (ret);
} // set()


/**
 * @brief Activate the PWMOutElement.
 */
void PWMOutElement::start()
{
  if (_pin < 0) {
    LOGGER_EERR("no pin");

  } else {
    // enable output and stay off
    Element::start();

#if defined(ESP8266)
    pinMode(_pin, OUTPUT);
    analogWriteRange(_range);

#elif (defined(ESP32))
    _channel = _board->nextLedChannel++;
    ledcSetup(_channel, 8000, 8);
    ledcAttachPin(_pin, _channel);
#endif

    _setValue(_value);

  } // if
} // start()


void PWMOutElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
  callback(PROP_VALUE, _printInteger(_value));
} // pushState()


/**
 * @brief set the physical level based on _inverse
 * @param logicalHigh
 */
void PWMOutElement::_setValue(int newValue)
{
  // TRACE("setValue(%d)", newValue);
  _value = newValue;

  if (active) {
    int v  = (_inverse) ? _range - _value : _value;
#if defined(ESP8266)
    analogWrite(_pin, v);
#elif (defined(ESP32))
    ledcWrite(_channel, v);
#endif

  } // if
} // _setValue


// End
