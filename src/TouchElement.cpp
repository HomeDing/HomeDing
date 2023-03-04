/**
 * @file TouchElement.cpp
 *
 * @brief Input Element for the HomeDing Library enabling the Touch functionality
 * by the ESP32 processors.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license,
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino.
 *
 * Changelog: see TouchElement.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <TouchElement.h>

#define TRACE(...)  // LOGGER_ETRACE(__VA_ARGS__)

/**
 * @brief static factory function to create a new TouchElement.
 * @return TouchElement* as Element* created element
 */
Element *TouchElement::create() {
  return (new TouchElement());
}  // create()



#if defined(ESP32)

// in case there is no touchRead() available on ESP32 boards (e.g. C3)
int __attribute__((weak)) touchRead(UNUSED int _pin) {
  return (0);
};

/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool TouchElement::set(const char *name, const char *value) {
  bool ret = true;

  if (Element::set(name, value)) {
    // done.

  } else if (_stricmp(name, "pin") == 0) {
    _pin = _atopin(value);

  } else if (_stricmp(name, "reference") == 0) {
    _reference = _atoi(value);

  } else if (_stricmp(name, "onhigh") == 0) {
    _highAction = value;

  } else if (_stricmp(name, "onlow") == 0) {
    _lowAction = value;

  } else if (_stricmp(name, "onvalue") == 0) {
    _valueAction = value;

  } else {
    ret = false;
  }  // if

  return (ret);
}  // set()


/**
 * @brief Activate the TouchElement.
 */
void TouchElement::start() {
  TRACE("start (%d)", _pin);
  // only start with valid pin as input.
  if (_pin >= 0) {
    Element::start();
  }  // if
}  // start()


/**
 * @brief check the state of the input.
 */
void TouchElement::loop() {
  int val = touchRead(_pin);

  // Serial.println(val);
  val = (val < 65);

  if (val != _value) {
    // TRACE("output %d->%d)", _value, val);
    _board->dispatch(val ? _highAction : _lowAction);
    _board->dispatch(_valueAction, val ? "1" : "0");
    _value = val;
  }  // if
}  // loop()


void TouchElement::pushState(
  std::function<void(const char *pName, const char *eValue)> callback) {
  Element::pushState(callback);
  callback("value", _printBoolean(_value));
}  // pushState()

#endif
// End
