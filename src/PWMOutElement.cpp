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
#include <Element.h>
#include <Board.h>

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

  } else if (_stricmp(name, "pin") == 0) {
    _pin = _atopin(value);

  } else if (_stricmp(name, "inverse") == 0) {
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
    LOGGER_EERR("no meaningful pin");

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
  callback(PROP_VALUE, String(_value).c_str());
} // pushState()


/**
 * @brief set the physical level based on _inverse
 * @param logicalHigh
 */
void PWMOutElement::_setValue(int newValue)
{
  LOGGER_ETRACE("setValue(%d)", newValue);
  _value = newValue;

  if (active) {
    analogWrite(_pin, (_inverse) ? _range - _value : _value);
  } // if
} // _setValue


// End
