/**
 * @file TM1637Element.cpp
 * @brief The P8913Element implements communication with P9813 LED driver chips to implement RGB lights.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD 3-Clause style license,
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:see TM1637Element.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include "TM1637Element.h"


#define TRACE(...) // LOGGER_ETRACE(__VA_ARGS__)

// from: https://github.com/pjpmarques/ChainableLED/blob/master/ChainableLED.cpp

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new TM1637Element
 * @return TM1637Element* created element
 */
Element *TM1637Element::create() {
  return (new TM1637Element());
} // create()


/* ===== Element functions ===== */

/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool TM1637Element::set(const char *name, const char *value) {
  TRACE("set %s=%s", name, value);
  bool ret = true;

  if (_stricmp(name, "value") == 0) {
    _value = value;
    _needUpdate = true;

  } else if (_stricmp(name, "brightness") == 0) {
    int b = _atoi(value);
    b = constrain(b, 0, 8);
    if (b != _brightness) {
      _brightness = b;
      _needUpdate = true;
    }

  } else if (_stricmp(name, "datapin") == 0) {
    _dataPin = _atopin(value);

  } else if (_stricmp(name, "clockpin") == 0) {
    _clockPin = _atopin(value);
  } else {
    ret = Element::set(name, value);
  } // if
  return (ret);
} // set()


/**
 * @brief Activate the TM1637Element.
 */
void TM1637Element::start() {
  TRACE("start()");
  _disp = new TM1637(_clockPin, _dataPin);
  if (_disp) {
    Element::start();
    _needUpdate = true;
  }
} // start()


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void TM1637Element::loop() {
  // do something
  if (_needUpdate) {
    _disp->setBrightness(_brightness);
    _disp->display(_value);
    _needUpdate = false;
  }
} // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void TM1637Element::pushState(
    std::function<void(const char *pName, const char *eValue)> callback) {
  Element::pushState(callback);
  callback(PROP_VALUE, _value.c_str());
} // pushState()


/* ===== Register the Element ===== */

// As long as the Element is project specific or is a element always used
// the registration is placed here without using a register #define.

// When transferred to the HomeDing library a #define like the
// HOMEDING_INCLUDE_XXX should be used to allow the sketch to select the
// available Elements. See <HomeDing.h> the move these lines to TM1637Element.h:

// #ifdef HOMEDING_REGISTER
// Register the TM1637Element onto the ElementRegistry.
bool TM1637Element::registered =
    ElementRegistry::registerElement("tm1637", TM1637Element::create);
// #endif

// End
