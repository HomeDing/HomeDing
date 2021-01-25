/**
 * @file LightElement.cpp
 * @brief Output Element to drive LEDs on GPIO pins.
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
 * Changelog:see LightElement.h
 */

#include <Arduino.h>
#include <Board.h>
#include <Element.h>

#include "LightElement.h"


void LightElement::setOutput(String value)
{
  uint32_t col = _atoColor(value.c_str());
  for (int n = _count - 1; n >= 0; n--) {
    int c = col & 0x00FF;
    col = col >> 8;
    analogWrite(_pins[n], c * brightness / 100);
    // TRACE("val[%d]=%d", n, c);
  } // for
} // setOutput()


/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new LightElement
 * @return LightElement* created element
 */
Element *LightElement::create()
{
  return (new LightElement());
} // create()


/* ===== Element functions ===== */

/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool LightElement::set(const char *name, const char *pValue)
{
  bool ret = true;
  // TRACE("set %s=%s", name, pValue);

  if (_stricmp(name, PROP_VALUE) == 0) {
    value = pValue;
    needUpdate = true;

  } else if (_stricmp(name, "enable") == 0) {
    enabled = _atob(pValue);
    needUpdate = true;

  } else if (_stricmp(name, "brightness") == 0) {
    brightness = _atoi(pValue);
    if (brightness < 0)
      brightness = 0;
    if (brightness > 100)
      brightness = 100;
    needUpdate = true;

  } else if (_stricmp(name, PROP_PIN) == 0) {
    for (int n = 0; n < LightElement::MAXPINS; n++) {
      _count = n;
      String p = getItemValue(pValue, n);
      if (p.isEmpty()) {
        break;
      } // if
      _pins[n] = _atopin(p.c_str());
      // TRACE("pin[%d]=%d", n, _pins[n]);
    }

  } else if (_stricmp(name, PROP_DURATION) == 0) {
    duration = _atotime(pValue) * 1000; // in msecs.

  } else {
    ret = Element::set(name, pValue);
  } // if

  return (ret);
} // set()


/**
 * @brief Activate the LightElement.
 */
void LightElement::start()
{
  Element::start();

  analogWriteRange(256);
  for (int n = 0; n < 3; n++) {
    pinMode(_pins[n], OUTPUT);
  } // for

  needUpdate = true;
} // start()


/**
 * @brief Give some processing time to the Element to check for next actions.
_ */
void LightElement::loop()
{
  if (needUpdate) {
    // send value to setOutput
    setOutput(enabled ? value : "x00000000");
    needUpdate = false;
  } // if
} // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void LightElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
  callback("value", value.c_str());
  callback("enable", enabled ? "1" : "0");
  callback("brightness", String(brightness).c_str());
} // pushState()


// End
