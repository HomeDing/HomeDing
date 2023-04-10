/**
 * @file StripeElement.cpp
 * @brief Base Element for LED stripes.
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
 * Changelog:see StripeElement.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <light/StripeElement.h>
#include <light/ColorElement.h>

#define TRACE(...)  // LOGGER_ETRACE(__VA_ARGS__)

/** set a single color for all pixels in the stripe
 * execute directly, don't wait for loop().
 */
void StripeElement::setColor(uint32_t color, int brightness) {
  TRACE("stripe:setColor(x%08x, %d)", color, brightness);

  for (int n = 0; n < _count; n++) {
    pixels[n] = color;
  }
  _brightness = brightness;
  _mode = Mode::show;  // all pixels with the same color.
  show();
  needUpdate = false;
}  // setColor()


/* ===== Strip specific function ===== */

void StripeElement::show() {
#if 0  // enable for logging all colors in a stripe
  TRACE("stripe::show(%d)", _count);

  if ((_count > 0) && (pixels)) {
    for (int n = 0; n < _count; n++) {
      TRACE(" %08x", pixels[n]);
    }
  }
#endif
}  // show()


/* ===== Element functions ===== */

/** set initial/default values on properties. */
void StripeElement::init(Board *board) {
  LightElement::init(board);

  // set defaults:
  value = "0";
}  // init()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool StripeElement::set(const char *name, const char *pValue) {
  TRACE("stripe::set %s=%s", name, pValue);
  bool ret1 = LightElement::set(name, pValue);
  bool ret2 = true;

  if (_stricmp(name, "value") == 0) {
    // saving to LightElement::value was handled in LightElement
    _mode = Mode::fix;

  } else if (_stricmp(name, "mode") == 0) {
    Mode m = (Mode)ListUtils::indexOf(StripeElement_ModeList, pValue);
    if ((m >= Mode::_min) && (m <= Mode::_max)) {
      _mode = m;
    }  // if
    needUpdate = true;

  } else if (_stricmp(name, "duration") == 0) {
    duration = _scanDuration(pValue);  // in msecs.

  } else if (_stricmp(name, "effectLength") == 0) {
    effectLength = _atoi(pValue);

  } else if ((!active) && (_stricmp(name, "count") == 0)) {
    _count = _atoi(pValue);

  } else if (_stricmp(name, "datapin") == 0) {
    STRIPE_DATA_PIN = _atopin(pValue);

  } else if (_stricmp(name, "clockpin") == 0) {
    STRIPE_CLOCK_PIN = _atopin(pValue);

  } else {
    ret2 = false;  // not handled

  }  // if

  return (ret1 || ret2);
}  // set()


/**
 * @brief Activate the StripeElement.
 */
void StripeElement::start() {
  LightElement::start();

  TRACE("start count=%d", _count);

  pixels = (uint32_t *)malloc(sizeof(uint32_t) * _count);
  memset(pixels, 0, sizeof(uint32_t) * _count);
}  // start()


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void StripeElement::loop() {
  if (pixels && needUpdate) {

    if (_mode == Mode::show) {
      // no color changes.
      show();
      needUpdate = false;

    } else if (_mode == Mode::fix) {
      TRACE("fix...");
      // get colors from value
      int len = ListUtils::length(value);
      uint32_t colors[len];

      for (int n = 0; n < len; n++) {
        colors[n] = _atoColor(ListUtils::at(value, n).c_str());
      }

      // set all pixel to a color using the pattern
      for (int n = 0; n < _count; n++) {
        pixels[n] = colors[n % len];
      }
      show();
      needUpdate = false;

    } else if (_mode == Mode::flow && (duration > 0)) {
      TRACE("flow...");

      if (enabled) {
        // flowing color patterns
        unsigned long now = millis();  // current (relative) time in msecs.
        unsigned int hue = (now % duration) * ColorElement::MAX_HUE / duration;
        unsigned int delta = ColorElement::MAX_HUE / effectLength;

        for (uint16_t i = 0; i < _count; i++) {
          pixels[i] = ColorElement::hslColor(hue);
          hue += delta;
        }
        show();
      }
      needUpdate = enabled;
    }
  }  // if
}  // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void StripeElement::pushState(std::function<void(const char *pName, const char *eValue)> callback) {
  LightElement::pushState(callback);
  if (_mode == StripeElement::Mode::flow) {
    callback("mode", "flow");
  } else {
    callback("mode", "fix");
  }
  callback("duration", _printInteger(duration));
  callback("effectlength", _printInteger(effectLength));
}  // pushState()


// End
