/**
 * @file RotaryElement.cpp
 * @brief Element to capture increments and decrements for values using a rotary encoder.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license.
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:see RotaryElement.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <RotaryElement.h>
#include <RotaryEncoder.h>


/* ===== statics ===== */

/**
 * @brief The Instance of the Rotary Encoder library.
 * Because of the interrupt implementation ther can be only ONE rotary encoder used at the same time.
 */
RotaryEncoder *__encoder;

/**
 * @brief The interrupt service routine to check the signals from the rotary encoder
 */
IRAM_ATTR void __checkPosition(void) {
  __encoder->tick();  // just call tick() to check the state.
}


/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new RotaryElement
 * @return RotaryElement* created element
 */
Element *RotaryElement::create() {
  return (new RotaryElement());
}  // create()


/* ===== Element functions ===== */

/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool RotaryElement::set(const char *name, const char *value) {
  bool ret = true;

  if (name == HomeDing::Actions::Value) {
    _value = _atoi(value);
    if (__encoder) {
      __encoder->setPosition(_value);
    }

  } else if (name == HomeDing::Actions::Step) {
    _step = _atoi(value);

  } else if (_stricmp(name, "pin1") == 0) {
    _pin1 = _atopin(value);

  } else if (_stricmp(name, "pin2") == 0) {
    _pin2 = _atopin(value);

  } else if (name == HomeDing::Actions::OnValue) {
    _valueAction = value;

  } else {
    ret = Element::set(name, value);
  }  // if

  return (ret);
}  // set()


/**
 * @brief Activate the RotaryElement.
 */
void RotaryElement::start() {
  // TRACE("start()");

  // Verify parameters
  if ((_pin1 < 0) || (_pin2 < 0)) {
    LOGGER_EERR("no pin");

  } else {
    // TRACE("connect %d %d\n", _pin1, _pin2);
    __encoder = new (std::nothrow) RotaryEncoder(_pin1, _pin2);
    if (__encoder) {
      pinMode(_pin1, INPUT_PULLUP);
      pinMode(_pin2, INPUT_PULLUP);
      // delay(30);
      // attachInterrupt(digitalPinToInterrupt(_pin1), __checkPosition, CHANGE);
      // attachInterrupt(digitalPinToInterrupt(_pin2), __checkPosition, CHANGE);
      Element::start();
    }  // if
  }    // if
}  // start()


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void RotaryElement::loop() {
  // do something
  __encoder->tick();  // just call tick() to check the state.

  long newPos = __encoder->getPosition();
  if (newPos != _value) {
    // send an action with the delta
    HomeDing::Actions::push(_valueAction, _step * (newPos - _value));
  }
  _value = newPos;
}  // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void RotaryElement::pushState(
  std::function<void(const char *pName, const char *eValue)> callback) {
  Element::pushState(callback);
  callback(HomeDing::Actions::Value, _printInteger((int)_value));
}  // pushState()

// End
