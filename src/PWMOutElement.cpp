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

#include "PWMOutElement.h"
#include "ElementRegistry.h"

#define LOGGER_MODULE "PWMout"
#include "core/Logger.h"


/**
 * @brief static factory function to create a new PWMOutElement.
 * @return PWMOutElement* as Element* created element
 */
Element *PWMOutElement::create()
{
  LOGGER_TRACE("create()");
  return (new PWMOutElement());
} // create()


bool PWMOutElement::set(const char *name, const char *value)
{
  LOGGER_TRACE("set(%s:%s)", name, value);
  bool ret = true;

  if (_stricmp(name, "pin") == 0) {
    _pin = atoi(value);

  } else if (_stricmp(name, "inverse") == 0) {
    _invers = _atob(value);

  } else if (_stricmp(name, "range") == 0) {
    _range = atoi(value);

  } else if (_stricmp(name, "value") == 0) {
    _setValue(atoi(value));

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
    LOGGER_ERR("no meaningful pin");

  } else {
    // enable output and stay off
    Element::start();
    pinMode(_pin, OUTPUT);
    analogWriteRange(_range);
    _setValue(_value);
  } // if
} // start()


void PWMOutElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
  callback("value", String(_value).c_str());
} // pushState()


/**
 * @brief set the physical level based on _inverse
 * @param logicalHigh
 */
void PWMOutElement::_setValue(int newValue)
{
  LOGGER_TRACE("setValue(%d)", newValue);
  _value = newValue;

  if (active) {
    analogWrite(_pin, (_invers) ? _range - _value : _value);
  } // if
} // _setValue


// End
