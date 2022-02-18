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

#include <NeoElement.h>


void NeoElement::_setColors(String colList) {
  uint16_t pixels = _strip->numPixels();

  String sCol;
  uint32_t col;
  int n = 0;
  int def = 0;  // defined colors

  while ((colList.length() > 0) && (n < pixels)) {
    int p = colList.indexOf(',');
    if (p > 0) {
      sCol = colList.substring(0, p);
      colList.remove(0, p + 1);

    } else {
      sCol = colList;
      colList = (const char *)NULL;
    }  // if

    col = _atoColor(sCol.c_str());
    _strip->setPixelColor(n++, col);
  }  // while

  // at the end of the defined colors.
  // now repeat until all PixelColors are set
  def = n;
  while (n < pixels) {
    col = _strip->getPixelColor(n % def);
    _strip->setPixelColor(n++, col);
  }
  needUpdate = true;
}  // _setColors()


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
  Element::init(board);

  // set defaults:
  _mode = Mode::color;
  value = "0";
  _count = 8;
}  // init()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool NeoElement::set(const char *name, const char *pValue) {
  // TRACE("set %s=%s", name, value);
  bool ret = LightElement::set(name, pValue);

  if (_stricmp(name, PROP_VALUE) == 0) {
    // saving to LightElement::value was handled in LightElement
    _mode = Mode::color;
    if (_strip)
      _setColors(value);

  } else if (_stricmp(name, "mode") == 0) {
    if (_stricmp(pValue, "color") == 0)
      _mode = Mode::color;
    else if (_stricmp(pValue, "wheel") == 0)
      _mode = Mode::wheel;
    else if (_stricmp(pValue, "flow") == 0)
      _mode = Mode::flow;
    else if (_stricmp(pValue, "pulse") == 0)
      _mode = Mode::pulse;
    if (_strip) {
      _strip->setBrightness(brightness);
      needUpdate = true;
    }
    ret = true;  // not handled in LightElement

  } else if (_stricmp(name, PROP_BRIGHTNESS) == 0) {
    // saving to LightElement::brightness was handled in LightElement
    if (_strip) {
      _strip->setBrightness(brightness * 255 / 100);  // convert to 0...255
      if (_mode == Mode::color) {
        _setColors(value);  // re-submit colors
      }
    }

  } else if (_stricmp(name, "duration") == 0) {
    duration = _atotime(pValue) * 1000;  // in msecs.

  } else if (_stricmp(name, "count") == 0) {
    _count = _atoi(pValue);
    ret = true;  // not handled in LightElement
  }              // if

  return (ret);
}  // set()


/**
 * @brief Activate the NeoElement.
 */
void NeoElement::start() {
  LightElement::start();

  _strip = new (std::nothrow) Adafruit_NeoPixel(_count, _pins[0], NEO_GRB + NEO_KHZ800);
  if (_strip) {
    _strip->begin();
    _strip->setBrightness(brightness);
    _setColors(value);
  }  // if
  // TRACE("start %d,%d", (_strip != nullptr), brightness);
}  // start()


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void NeoElement::loop() {
  if (_strip) {
    if (needUpdate) {
      // some settings have been changed
      if (enabled) {
        if (_mode == Mode::color) {
          _setColors(value);
        }
      } else {
        _strip->clear();
      }
      _strip->show();
      needUpdate = false;

    } else if (enabled && (_mode != Mode::color) && (duration != 0)) {
      // dynamic color patterns
      unsigned long now = millis();  // current (relative) time in msecs.
      unsigned int hue = (now % duration) * 65536L / duration;

      if (_mode == Mode::wheel) {
        _strip->fill(_strip->ColorHSV(hue));

      } else if (_mode == Mode::flow) {
        uint16_t pixels = _strip->numPixels();

        for (uint16_t i = 0; i < pixels; i++) {
          _strip->setPixelColor(i, _strip->ColorHSV((hue + i * 256 * 5) % 65536));
        }

      } else if (_mode == Mode::pulse) {
        // 0...255...1 = 256+254 = 510 steps
        int b = (now % duration) * 510L / duration;
        if (b > 255)
          b = 510 - b;
        _strip->setBrightness(b * brightness / 100);
        _setColors(value);

      }  // if
      needUpdate = true;
    }
  }  // if
}  // loop()


// End
