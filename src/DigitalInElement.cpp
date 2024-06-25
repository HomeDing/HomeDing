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

#include <Arduino.h>
#include <HomeDing.h>

#include <DigitalInElement.h>

/**
 * @brief static factory function to create a new DigitalInElement.
 * @return DigitalInElement* as Element* created element
 */
Element *DigitalInElement::create() {
  return (new DigitalInElement());
}  // create()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool DigitalInElement::set(const char *name, const char *value) {
  bool ret = true;

  if (Element::set(name, value)) {
    // done

  } else if (name == HomeDing::Action::Pin) {
    _pin = _atopin(value);

  } else if (_stricmp(name, "invert") == 0) {
    _inverse = _atob(value);

  } else if (_stricmp(name, "pullup") == 0) {
    _pullup = _atob(value);

  } else if (name == HomeDing::Action::OnHigh) {
    _highAction = value;

  } else if (name == HomeDing::Action::OnLow) {
    _lowAction = value;

  } else if (name == HomeDing::Action::OnValue) {
    _valueAction = value;

  } else {
    ret = false;
  }  // if
  return (ret);
}  // set()


/** @brief Setup the Element. */
void DigitalInElement::setup() {
  // only start with valid pin as input.
  if (_pin >= 0) {
    pinMode(_pin, _pullup ? INPUT_PULLUP : INPUT);
  }
};

/**
 * @brief Activate the DigitalInElement.
 */
void DigitalInElement::start() {
  // TRACE("start (%d)", _pin);
  // only start with valid pin as input.
  if (_pin >= 0) {
    _lastInLevel = digitalRead(_pin);
    if (_inverse)
      _lastInLevel = !_lastInLevel;
    Element::start();
  }  // if
}  // start()


/**
 * @brief check the state of the input.
 */
void DigitalInElement::loop() {
  int lev = digitalRead(_pin);
  if (_inverse)
    lev = !lev;

  if (lev != _lastInLevel) {
    // TRACE("output %d->%d)", _lastInLevel, lev);
    _board->dispatch(lev ? _highAction : _lowAction);
    _board->dispatch(_valueAction, lev ? "1" : "0");
    _lastInLevel = lev;
  }  // if
}  // loop()


void DigitalInElement::pushState(
  std::function<void(const char *pName, const char *eValue)> callback) {
  Element::pushState(callback);
  callback("value", _printBoolean(_lastInLevel));
}  // pushState()

// End
