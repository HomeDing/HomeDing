/**
 * @file RotaryElement.cpp
 * @brief Element Template class.
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

#include "RotaryElement.h"
#include "ElementRegistry.h"

#define LOGGER_MODULE "rotary"
#include "core/Logger.h"

/* ===== statics ===== */

/**
 * @brief The Instance of the Rotary Encoder library.
 * Because of the interrupt implementation ther can be only one rotary encoder used at the same time.
 */
RotaryEncoder *__encoder;


/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new RotaryElement
 * @return RotaryElement* created element
 */
Element *RotaryElement::create()
{
  LOGGER_TRACE("create()");
  return (new RotaryElement());
} // create()


/* ===== Element functions ===== */

/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool RotaryElement::set(const char *name, const char *value)
{
  LOGGER_TRACE("set(%s, %s)", name, value);
  bool ret = true;

  if (_stricmp(name, "value") == 0) {
    _value = atoi(value);

  } else if (_stricmp(name, "pin1") == 0) {
    _pin1 = _atopin(value);

  } else if (_stricmp(name, "pin2") == 0) {
    _pin2 = _atopin(value);

  } else if (_stricmp(name, "onChange") == 0) {
    _changeAction = value;

  } else {
    ret = Element::set(name, value);
  } // if

  return (ret);
} // set()


/**
 * @brief Check the signals from the rotary encoder 
 */
void __checkPosition()
{
  __encoder->tick(); // just call tick() to check the state.
}

/**
 * @brief Activate the RotaryElement.
 */
void RotaryElement::start()
{
  LOGGER_TRACE("start()");

  // Verify parameters
  if ((_pin1 < 0) || (_pin2 < 0)) {
    LOGGER_ERR("pin values are required.");

  } else {
    LOGGER_INFO("connect %d %d\n", _pin1, _pin2);
    __encoder = new RotaryEncoder(_pin1, _pin2);
    pinMode(_pin1, INPUT_PULLUP);
    attachInterrupt(_pin1,  __checkPosition, CHANGE);
    pinMode(_pin2, INPUT_PULLUP);
    attachInterrupt(_pin2,  __checkPosition, CHANGE);
    Element::start();
  } // if
} // start()


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void RotaryElement::loop()
{
  // do something
  __encoder->tick(); // just call tick() to check the state.
  char tmp[10];

  long newPos = __encoder->getPosition();
  if (newPos != _value) {
    // send an action with the delta
    itoa(newPos - _value, tmp, sizeof(tmp));
    _board->dispatch(_changeAction, tmp);
  }
  _value = newPos;
} // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void RotaryElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
  callback("value", String(_value).c_str());
} // pushState()

// End
