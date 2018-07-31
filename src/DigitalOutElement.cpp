// -----
// DigitalOutElement.cpp - Library for creating a digital outgoing signal.
//
// Copyright (c) by Matthias Hertel, https://www.mathertel.de.
//
// This work is licensed under a BSD style license.
// See https://www.mathertel.de/License.aspx.
// More information on https://www.mathertel.de/Arduino
// -----
// Changelog: see DigitalOutElement.h
// -----

#include "DigitalOutElement.h"
#include "ElementRegistry.h"

#define LOGGER_MODULE "DigitalOut"
#include "Logger.h"


/**
 * @brief static factory function to create a new DigitalOutElement.
 * @return DigitalOutElement* as Element* created element
 */
Element *DigitalOutElement::create()
{
  LOGGER_TRACE("create()");
  return (new DigitalOutElement());
} // create()


bool DigitalOutElement::set(const char *name, const char *value)
{
  LOGGER_TRACE("set(%s:%s)", name, value);
  bool ret = true;

  if (_stricmp(name, "pin") == 0) {
    _pin = _atopin(value);

  } else if (_stricmp(name, "invers") == 0) {
    _invers = _atob(value);

  } else   if (_stricmp(name, "level") == 0) {
    _setLevel(_atob(value));

  } else if (_stricmp(name, "on") == 0) {
    _setLevel(true);

  } else if (_stricmp(name, "off") == 0) {
    _setLevel(false);

  } else {
    ret = Element::set(name, value);
  } // if
  return (ret);
} // set()


/**
 * @brief Activate the DigitalOutElement.
 */
void DigitalOutElement::start()
{
  if (_pin < 0) {
    LOGGER_ERR("no meaningful pin");

  } else {
    // enable output and stay off
    pinMode(_pin, OUTPUT);
    _setLevel(false);
    Element::start();
  } // if
} // start()


void DigitalOutElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
  callback("level", String(_lastLevel).c_str());
} // pushState()


/**
 * @brief set the physical level based on _inverse
 * @param logicalHigh
 */
void DigitalOutElement::_setLevel(bool logicalHigh)
{
  _lastLevel = (logicalHigh  ? HIGH : LOW);
  int physLevel = (_invers ? (! _lastLevel) : _lastLevel);
  digitalWrite(_pin, physLevel);
} // _setLevel

// End
