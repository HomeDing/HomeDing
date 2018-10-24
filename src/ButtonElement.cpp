/**
 * @file ButtonElement.cpp
 * @brief Input Element for the IoT Board Library typicaly used with momentary
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
#include <ElementRegistry.h>
#include <Board.h>

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

  if (_stricmp(name, "type") == 0) {
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
    _invers = _atob(value);

  } else if (_stricmp(name, "pullup") == 0) {
    _pullup = _atob(value);

  } else if (_stricmp(name, "onon") == 0) {
    _onAction = value;

  } else if (_stricmp(name, "onoff") == 0) {
    _offAction = value;

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
  if (_pin < 0) {
    LOGGER_EERR("no meaningful pin");

  } else {
    pinMode(_pin, _pullup ? INPUT_PULLUP : INPUT);
    _lastInLevel = digitalRead(_pin);
    Element::start();
  } // if
} // start()


/**
 * @brief check the state of the input.
 */
void ButtonElement::loop()
{
  int lev = digitalRead(_pin);

  if (lev != _lastInLevel) {
    _lastInLevel = lev;

    if (_invers)
      lev = !lev;

    if (_type == BUTTON_TYPE_TOGGLE) {
      if (lev) {
        return; // without dispatching on UP.
      } else {
        // toggle _lastOutLevel
        _lastOutLevel = !_lastOutLevel;
      }

    } else {
      _lastOutLevel = lev;
    }

    if (_lastOutLevel) {
      _board->dispatch(_onAction);
    } else {
      _board->dispatch(_offAction);
    } // if
  }
} // loop()


void ButtonElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
  callback("value", String(_lastOutLevel).c_str());
} // pushState()

// End
