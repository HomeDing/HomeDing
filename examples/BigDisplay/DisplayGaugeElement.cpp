/**
 * @file DisplayGaugeElement.cpp
 * @brief Output Element for displaying a gauge with a value on a display.
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
 * Changelog, see DisplayGaugeElement.h.
 */

#include <Arduino.h>
#include <HomeDing.h>

#include "DisplayGaugeElement.h"

#include <gfxDraw.h>
#include <gfxDrawWidget.h>

#define TRACE(...) LOGGER_ETRACE(__VA_ARGS__)

/**
 * @brief static factory function to create a new DisplayGaugeElement.
 * @return DisplayGaugeElement* as Element* created element
 */
Element *DisplayGaugeElement::create() {
  return (new DisplayGaugeElement());
}  // create()


DisplayGaugeElement::DisplayGaugeElement() {
  isOpaque = true;
}

void DisplayGaugeElement::start() {

  // calculate some helpful values

  _radiusOut = (box.x_max - box.x_min + 1) / 2;
  _radiusIn = (_radiusOut * 8) / 10;  // segment width = 20%


  /*
  int16_t a1 = 180 - (_degree / 2); // maximum angle
  int16_t a0 = 180 + (_degree / 2); // minimum angle

  int16_t av = a0 - ((_value - _min) * _degree / (_max-_min));  // angle of value

  p(v) = [gfxDraw::sin256(v), gfxDraw::cos256(v)] * r / 256;

  path =
  "M$x1,$y1"
  "A$ro $ro 0 0 1 $x2,$y2"
  "L$x3,$y3"
  "A$x4,$y4"
  "z"
  */

  needsDraw = true;
}

/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool DisplayGaugeElement::set(const char *name, const char *value) {
  bool ret = true;

  if (DisplayOutputElement::set(name, value)) {
    // x, y, w, h


  } else if (name == HomeDing::Actions::Value) {
    _value = strtof(value, nullptr);

  } else if (!active) {
    // setup parameters:

    if (strcmp(name, "pointer") == 0) {

    } else if (strcmp(name, "degree") == 0) {
      int v = _atoi(value);
      _degree = constrain(v, 0, 360);

    } else if (strcmp(name, "min") == 0) {
      _minValue = _atoi(value);

    } else if (strcmp(name, "max") == 0) {
      _maxValue = _atoi(value);

    } else {
      ret = false;
    }

  } else {
    ret = false;
  }  // if
  return (ret);
}  // set()


// void DisplayGaugeElement::_drawRangeSegment(const char *path, int16_t x, int16_t y) {
//   // TRACE("_drawRangeSegment()");

// }  // _drawRangeSegment()


void DisplayGaugeElement::_drawRangeSegment(int16_t a0, int16_t a1, uint32_t fillcolor) {
  // TRACE("_drawRangeSegment()");

  gfxDraw::gfxDrawWidget o;
  gfxDraw::Segment s;

  o.setStrokeColor(gfxDraw::ARGB_TRANSPARENT);
  o.setFillColor(gfxDraw::ARGB(fillcolor));
  o.move(box.x_min, box.y_min);

  // construct the gauge segment

  s.type = gfxDraw::Segment::Move;
  s.p[0] = 0;
  s.p[1] = 0;
  o.addSegment(s);

  s.type = gfxDraw::Segment::Arc;
  s.p[0] = 0;
  s.p[1] = 0;
  o.addSegment(s);

  s.type = gfxDraw::Segment::Line;
  s.p[0] = 0;
  s.p[1] = 0;
  o.addSegment(s);

  s.type = gfxDraw::Segment::Arc;
  s.p[0] = 0;
  s.p[1] = 0;
  o.addSegment(s);

  s.type = gfxDraw::Segment::Close;
  o.addSegment(s);

  o.draw([&](int16_t x, int16_t y, gfxDraw::ARGB color) {
    _display->drawPixel(x, y, color.toColor24());
  });

}  // _drawRangeSegment()

/// @brief Draw this output element.
void DisplayGaugeElement::draw() {
  // TRACE("draw()");

  DisplayOutputElement::draw();  // set output colors

}  // draw()


// End
