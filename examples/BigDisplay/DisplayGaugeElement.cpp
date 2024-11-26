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
#include <gfxDrawGaugeWidget.h>

// enable TRACE for sending detailed output from this Element
#define TRACE(...) LOGGER_ETRACE(__VA_ARGS__)

/**
 * @brief static factory function to create a new DisplayGaugeElement.
 * @return DisplayGaugeElement* as Element* created element
 */
Element *DisplayGaugeElement::create() {
  return (new DisplayGaugeElement());
}  // create()


DisplayGaugeElement::DisplayGaugeElement() {
  TRACE("DisplayGaugeElement()");

  if (!_gWidget) {
    // prepare the widget with all properties.
    _gWidget = new gfxDraw::gfxDrawGaugeWidget();
  }
  if (!_gConfig) {
    _gConfig = new gfxDraw::GFXDrawGaugeConfig();
    // default boundaries
    _gConfig->minValue = 0,
    _gConfig->maxValue = 100,
    _gConfig->minAngle = 30,
    _gConfig->maxAngle = 360 - 30;
  }
  isOpaque = false;
}  // constructor


void DisplayGaugeElement::start() {
  TRACE("start()");
  int16_t size = box.qsize();

  TRACE("qSize = %d", size);

  if (size > 24) {
    _gConfig->x = box.x_min;
    _gConfig->y = box.y_min;
    _gConfig->w = size;

    _gConfig->minValue = 0;
    _gConfig->maxValue = 100;

    _gWidget->setConfig(_gConfig);

    isOpaque = false;
    DisplayOutputElement::start();
  }
}  // start()


/// @brief Set a parameter or property to a new value or start an action.
bool DisplayGaugeElement::set(const char *name, const char *value) {
  bool ret = true;

  if (DisplayOutputElement::set(name, value)) {
    // x, y, w, h

  } else if (!active) {
    // setup parameters:

    if (name == HomeDing::Actions::Min) {
      _gConfig->minValue = strtof(value, nullptr);

    } else if (name == HomeDing::Actions::Max) {
      _gConfig->maxValue = strtof(value, nullptr);

    } else if (strcmp(name, "minangle") == 0) {
      int v = _atoi(value);
      _gConfig->minAngle = constrain(v, 0, 360);

    } else if (strcmp(name, "maxangle") == 0) {
      int v = _atoi(value);
      _gConfig->maxAngle = constrain(v, 0, 360);

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


// void DisplayGaugeElement::_drawRangeSegment(int16_t a0, int16_t a1, uint32_t fillcolor) {
//   TRACE("_drawRangeSegment()");

//   gfxDraw::gfxDrawGaugeWidget o;
//   gfxDraw::Segment s;

//   o.setStrokeColor(gfxDraw::ARGB_TRANSPARENT);
//   o.setFillColor(gfxDraw::ARGB(fillcolor));
//   o.move(box.x_min, box.y_min);

//   // construct the gauge segment

//   s.type = gfxDraw::Segment::Move;
//   s.p[0] = 0;
//   s.p[1] = 0;
//   o.addSegment(s);

//   s.type = gfxDraw::Segment::Arc;
//   s.p[0] = 0;
//   s.p[1] = 0;
//   o.addSegment(s);

//   s.type = gfxDraw::Segment::Line;
//   s.p[0] = 0;
//   s.p[1] = 0;
//   o.addSegment(s);

//   s.type = gfxDraw::Segment::Arc;
//   s.p[0] = 0;
//   s.p[1] = 0;
//   o.addSegment(s);

//   s.type = gfxDraw::Segment::Close;
//   o.addSegment(s);

//   HomeDing::displayAdapter->startWrite();
//   o.draw([&](int16_t x, int16_t y, gfxDraw::ARGB color) {
//     HomeDing::displayAdapter->writePixel(x, y, color.toColor24());
//   });
//   HomeDing::displayAdapter->endWrite();

// }  // _drawRangeSegment()

/// @brief Draw this output element.
void DisplayGaugeElement::draw() {
  TRACE("draw()");

  _gWidget->setValue(strtof(_value.c_str(), nullptr));

  DisplayOutputElement::draw();  // set output colors

  // drawing using stroke and fill

  HomeDing::strokeColor = _strokeColor;
  HomeDing::fillColor = _backgroundColor;

  TRACE("a");
  HomeDing::displayAdapter->startWrite();
  TRACE("b");

  _gWidget->draw(HomeDing::stroke, HomeDing::fill);

  TRACE("c");
  HomeDing::displayAdapter->endWrite();
  TRACE("d");
}  // draw()


// End
