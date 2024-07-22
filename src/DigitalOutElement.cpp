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

#include <Arduino.h>
#include <HomeDing.h>

#include <DigitalOutElement.h>

/**
 * @brief static factory function to create a new DigitalOutElement.
 * @return DigitalOutElement* as Element* created element
 */
Element *DigitalOutElement::create() {
  DigitalOutElement *e = new DigitalOutElement();
  e->category = CATEGORY::Standard;  // no loop
  return (e);
}  // create()


bool DigitalOutElement::set(const char *name, const char *value) {
  bool ret = true;

  if (Element::set(name, value)) {
    // done

  } else if (name == HomeDing::Action::Value) {
    _setLevel(_atob(value));

  } else if (name == HomeDing::Action::Pin) {
    _pin = _atopin(value);

  } else if (_stricmp(name, "invert") == 0) {
    _inverse = _atob(value);

  } else if (_stricmp(name, "on") == 0) {
    _setLevel(true);

  } else if (_stricmp(name, "off") == 0) {
    _setLevel(false);

  } else {
    ret = false;
  }  // if
  return (ret);
}  // set()


/**
 * @brief Activate the DigitalOutElement.
 */
void DigitalOutElement::start() {
  if (_pin < 0) {
    LOGGER_EERR("no pin");

#if defined(ESP8266)
  } else if ((_pin >= 6) && (_pin <= 11)) {
    LOGGER_EERR("bad pin");

#elif defined(ESP32)
#if defined(CONFIG_IDF_TARGET_ESP32C3)
  } else if ((_pin >= 11) && (_pin <= 17)) {
    LOGGER_EERR("bad pin");
#endif
#endif

  } else {
    Element::start();
    // enable output and stay off
    pinMode(_pin, OUTPUT);
    _setLevel(_lastValue);
  }  // if
}  // start()


void DigitalOutElement::pushState(
  std::function<void(const char *pName, const char *eValue)> callback) {
  Element::pushState(callback);
  callback(HomeDing::Action::Value, _printBoolean(_lastValue));
}  // pushState()


/**
 * @brief set the physical level based on _inverse
 * @param logicalHigh
 */
void DigitalOutElement::_setLevel(bool logicalHigh) {
  _lastValue = (logicalHigh ? HIGH : LOW);
  int physLevel = (_inverse ? (!_lastValue) : _lastValue);
  if (active) {
    digitalWrite(_pin, physLevel);
  }
}  // _setLevel


// End