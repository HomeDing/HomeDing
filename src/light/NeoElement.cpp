/**
 * @file NeoElement.cpp
 * @brief Output Element to control neopixel / WS2812 based LED stripes on a GPIO pin.
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
 * Changelog:see NeoElement.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <light/NeoElement.h>

#define TRACE(...) LOGGER_ETRACE(__VA_ARGS__)

/** set a single color for all neopixels.
 * execute directly, don't wait for loop().
 */
void NeoElement::setColor(uint32_t color, int brightness) {
  TRACE("setColor(x%08x, %d)", color, brightness);
  LightElement::setColor(color, brightness);
  _mode = Mode::fix;  // turn off animations.

  if (needUpdate) {
    if (!enabled) {
      color = 0;
    }
    _strip->setBrightness(_brightness * 255 / 100);
    _strip->fill(color);
    _strip->show();
    needUpdate = false;
  }
}  // setColor()


/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new NeoElement
 * @return NeoElement* created element
 */
Element *NeoElement::create() {
  return (new NeoElement());
}  // create()


/* ===== Element functions ===== */

/** set initial/default values on properties. */
void NeoElement::init(Board *board) {
  LightElement::init(board);

  // set defaults:
  value = "0";
}  // init()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool NeoElement::set(const char *name, const char *pValue) {
  TRACE("set %s=%s", name, pValue);
  bool ret1 = LightElement::set(name, pValue);
  bool ret2 = true;

  if (_stricmp(name, "value") == 0) {
    // saving to LightElement::value was handled in LightElement
    _mode = Mode::fix;

  } else if (_stricmp(name, "mode") == 0) {
    Mode m = (Mode)ListUtils::indexOf("fix,flow", pValue);
    if ((m >= Mode::_min) && (m <= Mode::_max)) {
      _mode = m;
    }  // if

    if (_strip) {
      _strip->setBrightness(_brightness * 255 / 100);
      needUpdate = true;
    }

  } else if (_stricmp(name, "brightness") == 0) {
    // saving to LightElement::brightness was handled in LightElement
    if (_strip) {
      _strip->setBrightness(_brightness * 255 / 100);  // convert to 0...255
      needUpdate = true;
    }

  } else if (_stricmp(name, "duration") == 0) {
    duration = _scanDuration(pValue);  // in msecs.

  } else if ((!active) && (_stricmp(name, "count") == 0)) {
    _count = _atoi(pValue);

  } else if ((!active) && (_stricmp(name, "config") == 0)) {
    int n = ListUtils::indexOf("rgb,grb", pValue);
    if (n == 0) { _config = NEO_RGB; }
    if (n == 1) { _config = NEO_GRB; }

  } else {
    ret2 = false;  // not handled

  }  // if

  return (ret1 || ret2);
}  // set()

// Adafruit_NeoPixel spix = Adafruit_NeoPixel(32, D4, NEO_GRB + NEO_KHZ800);


/**
 * @brief Activate the NeoElement.
 */
void NeoElement::start() {
  LightElement::start();

  TRACE("start config=%04x count=%d pin=%d", _config, _count, _pins[0]);
  _strip = new (std::nothrow) Adafruit_NeoPixel(_count, _pins[0], _config | NEO_KHZ800);
  if (_strip) {
    _strip->begin();
    _strip->setBrightness(_brightness);
  }  // if
  TRACE("start %d,%d", (_strip != nullptr), _brightness);
}  // start()


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void NeoElement::loop() {
  if (_strip) {
    if ((needUpdate) && (_mode == Mode::fix)) {
      int len = ListUtils::length(value);
      uint32_t colors[len];

      for (int n = 0; n < len; n++) {
        colors[n] = _atoColor(ListUtils::at(value, n).c_str());
      }

      // set all PixelColors using pattern
      for (int n = 0; n < _count; n++) {
        _strip->setPixelColor(n, colors[n % len]);
      }
      _strip->show();

    } else if (enabled && (duration != 0) && (_mode == Mode::flow)) {
      // flowing color patterns
      unsigned long now = millis();  // current (relative) time in msecs.
      unsigned int hue = (now % duration) * 65536L / duration;

      for (uint16_t i = 0; i < _count; i++) {
        _strip->setPixelColor(i, _strip->ColorHSV((hue + i * 256 * 5) % 65536));
      }
      _strip->show();
    }
    needUpdate = false;
  }  // if
}  // loop()


// End
