/**
 * @file DigitalInElement.cpp
 * @brief Input Element for the IoT Board Library typically used with momentary
 * buttons or switches.
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license,
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino.
 *
 * Changelog: see DigitalInElement.h
 */

#include "DigitalInElement.h"
#include <Board.h>
#include <ElementRegistry.h>

// #include <OneButton.h>

/**
 * @brief static factory function to create a new DigitalInElement.
 * @return DigitalInElement* as Element* created element
 */
Element *DigitalInElement::create()
{
  return (new DigitalInElement());
} // create()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool DigitalInElement::set(const char *name, const char *value)
{
  // LOGGER_ETRACE("set(%s:%s)", name, value);
  bool ret = true;

  if (_stricmp(name, "pin") == 0) {
    _pin = _atopin(value);

  } else if (_stricmp(name, "inverse") == 0) {
    _inverse = _atob(value);

  } else if (_stricmp(name, "pullup") == 0) {
    _pullup = _atob(value);

  } else if (_stricmp(name, "onhigh") == 0) {
    _highAction = value;

  } else if (_stricmp(name, "onlow") == 0) {
    _lowAction = value;

  } else if (_stricmp(name, "onvalue") == 0) {
    _valueAction = value;

  } else {
    ret = Element::set(name, value);
  } // if
  return (ret);
} // set()


/**
 * @brief Activate the DigitalInElement.
 */
void DigitalInElement::start()
{
  // only start with valid pin as input.
  // LOGGER_ETRACE("start (%d)", _pin);
  if (_pin >= 0) {
    pinMode(_pin, _pullup ? INPUT_PULLUP : INPUT);
    _lastOutLevel = digitalRead(_pin);
    if (_inverse)
      _lastOutLevel = !_lastOutLevel;
    Element::start();
  } // if
} // start()


/**
 * @brief check the state of the input.
 */
void DigitalInElement::loop()
{
  if (_pin >= 0) {
    int lev = digitalRead(_pin);
    if (_inverse)
      lev = !lev;

    if (lev != _lastOutLevel) {
      // LOGGER_ETRACE("output %d->%d)", _lastOutLevel, lev);
      if (lev) {
        _board->dispatch(_highAction);
        _board->dispatch(_valueAction, "1");

      } else {
        _board->dispatch(_lowAction);
        _board->dispatch(_valueAction, "0");
      } // if
      _lastOutLevel = lev;
    } // if
  } // if
} // loop()


void DigitalInElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
  callback("value", String(_lastOutLevel).c_str());
} // pushState()

// End
