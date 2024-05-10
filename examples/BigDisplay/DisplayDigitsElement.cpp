/**
 * @file DisplayDigitsElement.cpp
 * @brief Output Element for displaying 7 segment digits on a display.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license,
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino.
 *
 * Changelog, see DisplayDigitsElement.h.
 */

#include <Arduino.h>
#include <HomeDing.h>

#include "DisplayDigitsElement.h"

#define TRACE(...)  // LOGGER_ETRACE(__VA_ARGS__)

#define DIGITS_WIDTH 17
#define DIGITS_GAP 2
#define DIGITS_HEIGHT 33

/**
 * @brief static factory function to create a new DisplayDigitsElement.
 * @return DisplayDigitsElement* as Element* created element
 */
Element *DisplayDigitsElement::create() {
  return (new DisplayDigitsElement());
}  // create()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool DisplayDigitsElement::set(const char *name, const char *value) {
  TRACE("set %s=%s", name, value);
  bool ret = true;

  if (DisplayOutputElement::set(name, value)) {
    // done

  } else if (name == HomeDing::Action::Value) {
    _value = value;
    needsDraw = true;

  } else {
    ret = false;
  }  // if
  return (ret);
}  // set()


void DisplayDigitsElement::_drawDigit(const char *path, int16_t x, int16_t y) {
  gfxDraw::gfxDrawObject *o = new gfxDraw::gfxDrawObject();

  o->setStrokeColor((_borderColor == RGB_TRANSPARENT ? gfxDraw::TRANSPARENT : gfxDraw::RGBA(_borderColor)));
  o->setFillColor((_color == RGB_TRANSPARENT ? gfxDraw::TRANSPARENT : gfxDraw::RGBA(_color)));

  o->setPath(path);
  o->scale(_scale);
  o->move(x, y);

  o->draw([&](int16_t x, int16_t y, gfxDraw::RGBA color) {
    _display->drawPixel(x, y, color.toColor24());
  });
}

void DisplayDigitsElement::_drawDigits7(char ch, int16_t x, int16_t y) {
  uint16_t drawSegments = 0b00000000;  // space and all unknown chars

  if (isDigit(ch)) {
    uint8_t SD[10] = {
      0b01110111,  // 0
      0b00100100,  // 1
      0b01011101,  // 2
      0b01101101,  // 3
      0b00101110,  // 4
      0b01101011,  // 5
      0b01111011,  // 6
      0b00100101,  // 7
      0b01111111,  // 8
      0b01101111,  // 9
    };
    drawSegments = SD[ch - '0'];

  } else if (ch == '-') {
    drawSegments = 0b00001000;

  } else if (ch == ':') {
    drawSegments = 0b10000000;  // ':' segments
  }

  if (drawSegments) {
    if (drawSegments & 0b00000001) _drawDigit("M1 0h14l-4 4h-6z", x, y);
    if (drawSegments & 0b00000010) _drawDigit("M0 1l4 4v6l-4 4z", x, y);
    if (drawSegments & 0b00000100) _drawDigit("M16 1v14l-4 -4v-6z", x, y);
    if (drawSegments & 0b00001000) _drawDigit("M1 16l2 -2h10l2 2l-2 2h-10z", x, y);
    if (drawSegments & 0b00010000) _drawDigit("M0 17l4 4v6l-4 4z", x, y);
    if (drawSegments & 0b00100000) _drawDigit("M16 17v14l-4 -4v-6z", x, y);
    if (drawSegments & 0b01000000) _drawDigit("M1 32l4 -4h6l4 4z", x, y);
    if (drawSegments & 0b10000000) _drawDigit("M6 6h4v4h-4zM6 22h4v4h-4z", x, y);
  }
}  // _drawDigits7()


/// @brief Draw this output element.
void DisplayDigitsElement::draw() {
  // TRACE("draw('%s' color=%08lx border=%08lx back=%08lx)", _value.c_str(), _color, _borderColor, _backgroundColor);
  int16_t x = box.x_min;

  DisplayOutputElement::draw();  // set output colors

  _scale = ((box.y_max - box.y_min + 1) * 100) / DIGITS_HEIGHT;

  for (int n = 0; n < _value.length(); n++) {
    if (_value[n] != _lastValue[n]) { _drawDigits7(_value[n], x, box.y_min); }
    x += GFXSCALE100(DIGITS_WIDTH + DIGITS_GAP, _scale);
  }
  _lastValue = _value;

  box.x_max = x - GFXSCALE100(DIGITS_GAP, _scale);
}  // draw()


// End
