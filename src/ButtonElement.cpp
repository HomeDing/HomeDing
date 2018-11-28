/**
 * @file ButtonElement.cpp
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
 * Changelog: see ButtonElement.h
 */

#include "ButtonElement.h"
#include <Board.h>
#include <ElementRegistry.h>

// #include <OneButton.h>

/**
 * @brief static factory function to create a new ButtonElement.
 * @return ButtonElement* as Element* created element
 */
Element *ButtonElement::create()
{
  return (new ButtonElement());
} // create()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool ButtonElement::set(const char *name, const char *value)
{
  LOGGER_ETRACE("set(%s:%s)", name, value);
  bool ret = true;

  if (_stricmp(name, "value") == 0) {
    _webLevel = _atob(value);

  } else if (_stricmp(name, "type") == 0) {
    if (_stricmp(value, "level") == 0) {
      _type = BUTTON_TYPE_LEVEL;
    } else if (_stricmp(value, "toggle") == 0) {
      _type = BUTTON_TYPE_TOGGLE;
    } else {
      LOGGER_EERR("unknown type");
    }

  } else if (_stricmp(name, "pin") == 0) {
    _pin = _atopin(value);

  } else if (_stricmp(name, "inverse") == 0) {
    _inverse = _atob(value);

  } else if (_stricmp(name, "pullup") == 0) {
    _pullup = _atob(value);

  } else if (_stricmp(name, "onon") == 0) {
    _onAction = value;

  } else if (_stricmp(name, "onoff") == 0) {
    _offAction = value;

  } else if (_stricmp(name, "onvalue") == 0) {
    _valueAction = value;

  } else {
    ret = Element::set(name, value);
  } // if
  return (ret);
} // set()


/**
 * @brief Activate the ButtonElement.
 */
void ButtonElement::start()
{
  if (_pin >= 0) {
    pinMode(_pin, _pullup ? INPUT_PULLUP : INPUT);
    // read the current level
    _lastPinLevel = digitalRead(_pin);
    if (_inverse)
      _lastPinLevel = !_lastPinLevel;
  } else {
    _lastPinLevel = 0;
  } // if
  _lastWebLevel = 0;

  Element::start();
} // start()


/**
 * @brief check the state of the input.
 */
void ButtonElement::loop()
{
  if (_pin >= 0) {
    int lev = digitalRead(_pin);
    if (_inverse)
      lev = !lev;

    if ((_type == BUTTON_TYPE_TOGGLE) && (_lastPinLevel > lev)) {
      // toggle _outLevel
      _outLevel = !_outLevel;
    }
    _lastPinLevel = lev;
  } // if

  if (_type == BUTTON_TYPE_TOGGLE) {
    if (_lastWebLevel > _webLevel) {
      // toggle _outLevel
      _outLevel = !_outLevel;
    } // if
    _lastWebLevel = _webLevel;

  } else if (_type == BUTTON_TYPE_LEVEL) {
    _outLevel = (_lastPinLevel || _lastWebLevel);
  } // if

  if (_outLevel != _lastOutLevel) {
    // LOGGER_ETRACE("output level=%d (%d,%d)", _outLevel, _lastPinLevel, _lastWebLevel);
    if (_outLevel) {
      _board->dispatch(_onAction);
      _board->dispatch(_valueAction, "1");

    } else {
      _board->dispatch(_offAction);
      _board->dispatch(_valueAction, "0");
    } // if
    _lastOutLevel = _outLevel;
  } // if
} // loop()


void ButtonElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
  callback("value", String(_lastOutLevel).c_str());
} // pushState()

// End
