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
#include <HomeDing.h>

#include <light/LightElement.h>

#define TRACE(...) // LOGGER_ETRACE(__VA_ARGS__)

/**
 * @brief Construct a new LightElement.
 */
LightElement::LightElement() {
  startupMode = Element_StartupMode::System;
}


void LightElement::setColors(uint32_t *color, int brightness) {
  // just take the first in the array as default.
  setColor(*color, brightness);
} // setColors


// set output
void LightElement::setColor(uint32_t color, int brightness) {
  TRACE("setColor(#%08x, %d)", color, brightness);
  char colBuffer[16];

  brightness = constrain(brightness, 0, 100);
  _brightness = brightness;

  snprintf(colBuffer, sizeof(colBuffer), "#%08x", color);
  value = colBuffer;

  if (!enabled) {
    color = 0;
  }

  for (int n = _count - 1; n >= 0; n--) {
    int c = color & 0x00FF;

#if defined(ESP8266)
    analogWrite(_pins[n], c * brightness / 100);
#elif (defined(ESP32))
    ledcWrite(_channels[n], c * brightness / 100);
#endif

    TRACE("%d pin=%d value=%02x", n, _pins[n], c);
    color = color >> 8;
  }  // for
}  // setColor()


/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new LightElement
 * @return LightElement* created element
 */
Element *LightElement::create() {
  return (new LightElement());
}  // create()


/* ===== Element functions ===== */

/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool LightElement::set(const char *name, const char *pValue) {
  bool ret = true;
  TRACE("set %s=%s", name, pValue);

  if (_stricmp(name, "value") == 0) {
    value = pValue;
    needUpdate = true;

  } else if (_stricmp(name, "enable") == 0) {
    enabled = _atob(pValue);
    needUpdate = true;

  } else if (_stricmp(name, "brightness") == 0) {
    _brightness = _atoi(pValue);
    _brightness = constrain(_brightness, 0, 100);
    needUpdate = true;

  } else if (_stricmp(name, "pin") == 0) {
    _count = 0;
    while (_count < LightElement::MAXPINS) {
      String p = getItemValue(pValue, _count);
      if (p.isEmpty()) {
        break;
      }  // while
      _pins[_count] = _atopin(p.c_str());
      TRACE("pin[%d]=%d", _count, _pins[_count]);
      _count++;
    }  // while

  } else {
    ret = Element::set(name, pValue);
  }  // if

  return (ret);
}  // set()


/**
 * @brief Activate the LightElement.
 */
void LightElement::start() {
  Element::start();

#if defined(ESP8266)
  analogWriteRange(256);
#endif

  for (int n = 0; n < _count; n++) {
#if defined(ESP8266)
    pinMode(_pins[n], OUTPUT);
#elif (defined(ESP32))
    _channels[n] = _board->nextLedChannel++;
    ledcSetup(_channels[n], 8000, 8);
    ledcAttachPin(_pins[n], _channels[n]);
#endif
  }  // for

  needUpdate = true;
  loop();
}  // start()


/**
 * @brief Give some processing time to the Element to check for next actions.
_ */
void LightElement::loop() {
  if (needUpdate) {
    uint32_t col = _atoColor(value.c_str());
    setColor(col, _brightness);
    needUpdate = false;
  }  // if
}  // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void LightElement::pushState(
  std::function<void(const char *pName, const char *eValue)> callback) {
  Element::pushState(callback);
  callback("value", value.c_str());
  callback("enable", enabled ? "1" : "0");
  callback("brightness", _printInteger(_brightness));
}  // pushState()


// End
