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

#define TRACE(...) LOGGER_ETRACE(__VA_ARGS__)

/** set a single color for all pixels in the stripe
 * execute directly, don't wait for loop().
 */
void StripeElement::setColor(uint32_t color, int brightness) {
  TRACE("setColor(x%08x, %d)", color, brightness);

  _mode = Mode::single;  // all pixels with the same color.
  for (int n = 0; n < _count; n++) {
    pixels[n] = color;
  }
  _brightness = brightness;
  // needUpdate = false;
  show();
}  // setColor()


/* ===== Strip specific function ===== */

void StripeElement::show() {
  TRACE("show(%d)", _count);

  if ((_count > 0) && (pixels)) {
    for (int n = 0; n < _count; n++) {
      TRACE(" %08x", pixels[n]);
    }
  }
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
  TRACE("set %s=%s", name, pValue);
  bool ret1 = LightElement::set(name, pValue);
  bool ret2 = true;

  if (_stricmp(name, "value") == 0) {
    // saving to LightElement::value was handled in LightElement
    _mode = Mode::fix;

  } else if (_stricmp(name, "mode") == 0) {
    Mode m = (Mode)ListUtils::indexOf("fix,single,flow", pValue);
    if ((m >= Mode::_min) && (m <= Mode::_max)) {
      _mode = m;
    }  // if
    needUpdate = true;

  } else if (_stricmp(name, "duration") == 0) {
    duration = _scanDuration(pValue);  // in msecs.

  } else if ((!active) && (_stricmp(name, "count") == 0)) {
    _count = _atoi(pValue);

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

  TRACE("start config=%04x count=%d pin=%d", _config, _count, _pins[0]);

  pixels = (uint32_t *)malloc(sizeof(uint32_t) * _count);
  memset(pixels, 0, sizeof(uint32_t) * _count);


}  // start()


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void StripeElement::loop() {
  if ((pixels) && (needUpdate)) {

    if (_mode == Mode::fix) {
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

    } else if (_mode == Mode::flow && (duration > 0)) {
      // flowing color patterns
      unsigned long now = millis();  // current (relative) time in msecs.
      unsigned int hue = (now % duration) * 65536L / duration;

      for (uint16_t i = 0; i < _count; i++) {
        // pixels[i] = ColorHSV((hue + i * 256 * 5) % 65536);
      }

      // } else if (_mode == Mode::single) {
      //   show();
    }
    show();
    needUpdate = false;
  }  // if
}  // loop()


// End
