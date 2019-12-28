/**
 * @file NeoElement.cpp
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
 * Changelog:see NeoElement.h
 */

#include <Arduino.h>
#include <Board.h>
#include <Element.h>

#include "NeoElement.h"


void NeoElement::_setColors(String colList)
{
  uint16_t pixels = _strip->numPixels();

  String sCol;
  uint32_t col;
  int n = 0;
  int def = 0; // defined colors

  while ((colList.length() > 0) && (n < pixels)) {
    int p = colList.indexOf(',');
    if (p > 0) {
      sCol = colList.substring(0, p);
      colList.remove(0, p + 1);

    } else {
      sCol = colList;
      colList = (const char *)NULL;
    } // if

    col = _atoColor(sCol.c_str());
    _strip->setPixelColor(n++, col);
  } // while

  // at the end of the defined colors.
  // now repeat untill all PixelColors are set
  def = n;
  while (n < pixels) {
    col = _strip->getPixelColor(n % def);
    _strip->setPixelColor(n++, col);
  }
  _needShow = true;
}


/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new NeoElement
 * @return NeoElement* created element
 */
Element *NeoElement::create()
{
  return (new NeoElement());
} // create()


/* ===== Element functions ===== */

/** set initial/default values on properties. */
void NeoElement::init(Board *board)
{
  Element::init(board);

  // set defaults:
  _mode = Mode::color;
  _colors = "0";
  _pin = 2;
  _count = 8;
  _brightness = 64;
  _duration = 4000;
  _needShow = false;
} // init()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool NeoElement::set(const char *name, const char *value)
{
  bool ret = true;

  if (_stricmp(name, PROP_VALUE) == 0) {
    _mode = Mode::color;
    _colors = value;
    if (_strip)
      _setColors(value);

  } else if (_stricmp(name, "mode") == 0) {
    if (_stricmp(value, "color") == 0)
      _mode = Mode::color;
    else if (_stricmp(value, "wheel") == 0)
      _mode = Mode::wheel;
    else if (_stricmp(value, "flow") == 0)
      _mode = Mode::flow;
    else if (_stricmp(value, "pulse") == 0)
      _mode = Mode::pulse;

  } else if (_stricmp(name, "brightness") == 0) {
    _brightness = atoi(value);
    if (_strip) {
      _strip->setBrightness(_brightness);
      if (_mode == Mode::color) {
        _setColors(_colors);
      }
      _needShow = true;
    }

  } else if (_stricmp(name, PROP_PIN) == 0) {
    _pin = _atopin(value);

  } else if (_stricmp(name, "count") == 0) {
    _count = atoi(value);

  } else if (_stricmp(name, "duration") == 0) {
    _duration = _atotime(value) * 1000;

  } else {
    ret = Element::set(name, value);
  } // if

  return (ret);
} // set()


/**
 * @brief Activate the NeoElement.
 */
void NeoElement::start()
{
  Element::start();

  _strip = new Adafruit_NeoPixel(_count, _pin, NEO_GRB + NEO_KHZ400);
  if (_strip) {
    _strip->begin();
    _setColors(_colors);
    _strip->setBrightness(_brightness);
    _needShow = true;
  }
} // start()


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void NeoElement::loop()
{
  if (_strip) {
    if (_needShow) {
      // this time send to strip
      _strip->show();
      _needShow = false;

    } else if ((_duration != 0) && (_mode != Mode::color)) {
      unsigned long now = millis(); // current (relative) time in msecs.
      unsigned int hue = (now * 65536 / _duration) % 65536;

      if (_mode == Mode::wheel) {
        _strip->fill(_strip->ColorHSV(hue));
        _needShow = true;

      } else if (_mode == Mode::flow) {
        uint16_t pixels = _strip->numPixels();

        for (int i = 0; i < pixels; i++) {
          _strip->setPixelColor(i, _strip->ColorHSV((hue + i * 256 * 5) % 65536));
        }

      } else if (_mode == Mode::pulse) {
        // 0...255...1 = 256+254 = 510 steps
        int b = (now * 510 / _duration) % 510;
        _strip->setBrightness(b > 255 ? 510 - b : b);
        _setColors(_colors);

      } // if
      _needShow = true;
    }
  } // if
} // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void NeoElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
  callback("value", _colors.c_str());
} // pushState()


// End
