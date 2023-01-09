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
}  // setColors


// set output
void LightElement::setColor(uint32_t color, int brightness) {
  TRACE("L-setColor(#%08x, %d)", color, brightness);
  char colBuffer[16];

  int b = constrain(brightness, 0, 100);
  if (b != _brightness) {
    _brightness = b;
    needUpdate = true;
  }

  if (color != _outColor) {
    snprintf(colBuffer, sizeof(colBuffer), "#%08x", color);
    value = colBuffer;
    _outColor = color;
    needUpdate = true;
  }

  if (needUpdate && pwmMode) {
    loop();
  }
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
  TRACE("L-set %s=%s", name, pValue);

  if (_stricmp(name, "value") == 0) {
    if (_stricmp(value.c_str(), pValue)) {
      value = pValue;
      _outColor = _atoColor(pValue);
      needUpdate = true;
    }

  } else if (_stricmp(name, "enable") == 0) {
    enabled = _atob(pValue);
    needUpdate = true;

  } else if (_stricmp(name, "mode") == 0) {
    if (_stricmp(pValue, "pwm") == 0)
      pwmMode = true;
    needUpdate = true;

  } else if (_stricmp(name, "brightness") == 0) {
    int b = _atoi(pValue);
    b = constrain(b, 0, 100);
    if (_brightness != b) {
      _brightness = b;
      needUpdate = true;
    }

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

  if (pwmMode) {
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
    loop();
    needUpdate = false;
  } else {
    needUpdate = true;
  }
}  // start()


/**
 * @brief Change color and brightness if needed.
 */
void LightElement::loop() {
  if ((needUpdate) && (pwmMode)) {

    if (!enabled) {
      _outColor = 0;
    }

    uint32_t color = _outColor;

    for (int n = _count - 1; n >= 0; n--) {
      int c = color & 0x00FF;

#if defined(ESP8266)
      analogWrite(_pins[n], c * _brightness / 100);
#elif (defined(ESP32))
      ledcWrite(_channels[n], c * _brightness / 100);
#endif

      TRACE("L-set(%d) pin=%d value=%02x", n, _pins[n], c);
      color = color >> 8;
    }  // for
  }    // if
  needUpdate = false;
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
