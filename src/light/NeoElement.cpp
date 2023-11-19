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

// https://github.com/adafruit/Adafruit_NeoPixel
#include <Adafruit_NeoPixel.h>

#define TRACE(...)  // LOGGER_ETRACE(__VA_ARGS__)

void NeoElement::show() {
  TRACE("neo:show(%d) %d", _count, _brightness);
  // StripeElement::show();

  if (enabled) {
    _strip->setBrightness(_brightness * 255 / 100);

    // set all PixelColors using pattern
    for (int n = 0; n < _count; n++) {
      _strip->setPixelColor(n, pixels[n]);
    }

  } else {
    _strip->fill(0);
  }
  _strip->show();
}  // show()


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
  StripeElement::init(board);

  // set defaults:
  _config = NEO_GRB;
}  // init()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool NeoElement::set(const char *name, const char *pValue) {
  TRACE("set %s=%s", name, pValue);
  bool ret = true;

  if (StripeElement::set(name, pValue)) {
    // ok.

  } else if ((!active) && (_stricmp(name, "config") == 0)) {
    int n = ListUtils::indexOf("rgb,grb", pValue);
    if (n == 0) { _config = NEO_RGB; }
    if (n == 1) { _config = NEO_GRB; }

  } else {
    ret = false;  // not handled
  }               // if

  return (ret);
}  // set()


/**
 * @brief Activate the NeoElement.
 */
void NeoElement::start() {
  StripeElement::start();

  TRACE("start config=%04x count=%d pin=%d", _config, _count, STRIPE_DATA_PIN);
  _strip = new (std::nothrow) Adafruit_NeoPixel(_count, STRIPE_DATA_PIN, _config | NEO_KHZ800);
  if (_strip) {
    _strip->begin();
  }  // if
  TRACE("start %d,%d", (_strip != nullptr), _brightness);
}  // start()


// End
