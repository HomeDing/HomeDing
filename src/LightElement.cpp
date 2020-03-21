/**
 * @file LightElement.cpp
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
    LOGGER_ETRACE("val[%d]=%d", n, c);
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
bool LightElement::set(const char *name, const char *value)
{
  bool ret = true;
  LOGGER_ETRACE("set %s=%s", name, value);

  if (_stricmp(name, PROP_VALUE) == 0) {
    this->value = value;
    this->needUpdate = true;

  } else if (_stricmp(name, "brightness") == 0) {
    int b = _atoi(value);
    if ((b >= 0) && (b <= 100))
      brightness = b;
    needUpdate = true;

  } else if (_stricmp(name, PROP_PIN) == 0) {
    for (int n = 0; n < LightElement::MAXPINS; n++) {
      _count = n;
      String p = getItemValue(value, n);
      if (p.isEmpty()) {
        break;
      } // if
      _pins[n] = _atopin(p.c_str());
      LOGGER_ETRACE("pin[%d]=%d", n, _pins[n]);
    }

  } else if (_stricmp(name, "duration") == 0) {
    duration = _atotime(value) * 1000; // in msecs.

  } else {
    ret = Element::set(name, value);
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
    setOutput(value);
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
  callback("brightness", String(brightness).c_str());
} // pushState()


// End
