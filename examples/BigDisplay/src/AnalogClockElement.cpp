/**
 * @file AnalogClockElement.cpp
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:see AnalogClockElement.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <ctime>

#include "AnalogClockElement.h"

#include <gfxDraw.h>

// enable TRACE for sending detailed output from this Element
#define TRACE(...)  // LOGGER_ETRACE(__VA_ARGS__)

/* ===== Define local constants and often used strings ===== */

// like:
// #define ANYCONSTANT 1000


/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new AnalogClockElement
 * @return AnalogClockElement* created element
 */
Element *AnalogClockElement::create() {
  return (new AnalogClockElement());
}  // create()


/* ===== Element functions ===== */

AnalogClockElement::AnalogClockElement() {
  category = (CATEGORY)(CATEGORY::Widget | CATEGORY::Looping);  // needs loop
}


void AnalogClockElement::init(Board *board) {
  TRACE("init()");
  Element::init(board);
  // do something here like initialization
}  // init()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool AnalogClockElement::set(const char *name, const char *value) {
  bool ret = true;

  if (DisplayOutputElement::set(name, value)) {
    // done.

  } else if (_stricmp(name, "colors") == 0) {
    int count = ListUtils::length(value);

    if (count == 1) {
      _hand_color_h = _hand_color_m = _hand_color_s = _atoColor(ListUtils::at(value, 0).c_str());
    } else if (count == 2) {
      _hand_color_h = _hand_color_m = _atoColor(ListUtils::at(value, 0).c_str());
      _hand_color_s = _atoColor(ListUtils::at(value, 1).c_str());
    } else {
      _hand_color_h = _atoColor(ListUtils::at(value, 0).c_str());
      _hand_color_m = _atoColor(ListUtils::at(value, 1).c_str());
      _hand_color_s = _atoColor(ListUtils::at(value, 2).c_str());
    }

  } else if (_stricmp(name, "cx") == 0) {
    _cx = atoi(value);

  } else if (_stricmp(name, "cy") == 0) {
    _cy = atoi(value);

  } else if (_stricmp(name, "r") == 0) {
    _radius = atoi(value);

  } else {
    ret = false;
  }  // if

  box.x_min = _cx - _radius;
  box.x_max = _cx + _radius;
  box.y_min = _cy - _radius;
  box.y_max = _cy + _radius;

  return (ret);
}  // set()


/**
 * @brief Activate the AnalogClockElement.
 */
void AnalogClockElement::start() {
  TRACE("start()");
  DisplayOutputElement::start();
  _drawClock();
}  // start()


void AnalogClockElement::draw() {
  TRACE("draw()");
  DisplayOutputElement::draw();

  time_t now = time(nullptr);
  struct tm *lt = localtime(&now);
  time_t secs = lt->tm_sec + (lt->tm_min * 60) + (lt->tm_hour * 60 * 60);

  // calculate angles in 360°
  uint16_t aSec = lt->tm_sec * 6;
  uint16_t aHour = secs / 120;
  uint16_t aMin = (secs % (60 * 60)) / 10;

  // _gfx->startWrite();

  _drawHand(_last_a_sec, 9, 1, _backgroundColor);
  if (aMin != _last_a_min)
    _drawHand(_last_a_min, 8, 3, _backgroundColor);
  if (aHour != _last_a_hour)
    _drawHand(_last_a_hour, 8, 5, _backgroundColor);

  _drawHand(aHour, 6, 5, _hand_color_h);
  _drawHand(aMin, 8, 3, _hand_color_m);
  _drawHand(aSec, 9, 1, _hand_color_s);

  _last_a_sec = aSec;
  _last_a_min = aMin;
  _last_a_hour = aHour;
  HomeDing::displayAdapter->setFlush();
  _shown_time = now;
}

/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void AnalogClockElement::loop() {
  if ((!needsDraw) && (_shown_time != time(nullptr))) {
    TRACE("need");
    needsDraw = true;
    HomeDing::displayAdapter->setFlush();
  }
  DisplayOutputElement::loop();
}  // loop()


void AnalogClockElement::term() {
  TRACE("term()");
  Element::term();
}  // term()


void AnalogClockElement::_drawClock() {
  // LOGGER_ETRACE("drawClock()");
  float rad1 = (M_TWOPI / 60);
  int16_t x0, y0, x1, y1;
  BoundingBox box(_cx - _radius, _cy - _radius, _cx + _radius, _cy + _radius);

  HomeDing::displayAdapter->startWrite();

  HomeDing::strokeColor = _strokeColor;
  HomeDing::fillColor = _backgroundColor;

  gfxDraw::drawCircle(
    gfxDraw::Point((_x0 + _x1) / 2, (_y0 + _y1) / 2),
    (_x1 - _x0) / 2,
    HomeDing::stroke,
    HomeDing::fill);

  for (uint8_t i = 0; i < 60; i++) {
    float deg = (rad1 * i);

    x0 = sin(deg) * _radius;
    y0 = -cos(deg) * _radius;

    if ((i % 15) == 0) {
      x1 = (x0 * 9.0) / 12.0;
      y1 = (y0 * 9.0) / 12.0;
      HomeDing::strokeColor = RGB_BLUE;

    } else if ((i % 5) == 0) {
      x1 = (x0 * 10.0) / 12.0;
      y1 = (y0 * 10.0) / 12.0;
      HomeDing::strokeColor = RGB_GREEN;

    } else {
      x1 = (x0 * 11.5) / 12.0;
      y1 = (y0 * 11.5) / 12.0;
      HomeDing::strokeColor = RGB_GRAY;
    }

    gfxDraw::drawLine(_cx + x0, _cy + y0, _cx + x1, _cy + y1, HomeDing::stroke);

  }  // for
  HomeDing::displayAdapter->endWrite();
}

// void _drawLineWidth(DisplayAdapter *d, int x0, int y0, int x1, int y1, float wd)
// {                                    /* plot an anti-aliased line of width wd */
//    int dx = abs(x1-x0), sx = x0 < x1 ? 1 : -1;
//    int dy = abs(y1-y0), sy = y0 < y1 ? 1 : -1;
//    int err = dx-dy, e2, x2, y2;                           /* error value e_xy */
//    float ed = dx+dy == 0 ? 1 : sqrt((float)dx*dx+(float)dy*dy);

//    for (wd = (wd+1)/2; ; ) {                                    /* pixel loop */
//       setPixelColor(x0, y0, max(0,255*(abs(err-dx+dy)/ed-wd+1)));
//       e2 = err; x2 = x0;
//       if (2*e2 >= -dx) {                                            /* x step */
//          for (e2 += dy, y2 = y0; e2 < ed*wd && (y1 != y2 || dx > dy); e2 += dx)
//             setPixelColor(x0, y2 += sy, max(0,255*(abs(e2)/ed-wd+1)));
//          if (x0 == x1) break;
//          e2 = err; err -= dy; x0 += sx;
//       }
//       if (2*e2 <= dy) {                                             /* y step */
//          for (e2 = dx-e2; e2 < ed*wd && (x1 != x2 || dx < dy); e2 += dy)
//             setPixelColor(x2 += sx, y0, max(0,255*(abs(e2)/ed-wd+1)));
//          if (y0 == y1) break;
//          err += dx; y0 += sy;
//       }
//    }
// }

void _drawLineWidth(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color, uint16_t wd) {

  DisplayAdapter *da = HomeDing::displayAdapter;

  da->startWrite();
  if (wd < 2) {
    HomeDing::strokeColor = color;
    gfxDraw::drawLine(x0, y0, x1, y1, HomeDing::stroke);

  } else if (wd < 4) {
    //   X
    //  XOX
    //   X
    gfxDraw::drawLine(x0, y0, x1, y1, [da, color](int16_t x, int16_t y) {
      da->writePixel(x + 1, y, color);
      da->writePixel(x, y + 1, color);
      da->writePixel(x - 1, y, color);
      da->writePixel(x, y - 1, color);
    });

  } else if (wd < 6) {
    //   X       3
    //  X X     4 2
    // X O X   5 O 1
    //  X X     6 X
    //   X       X
    gfxDraw::drawLine(x0, y0, x1, y1, [da, color](int16_t x, int16_t y) {
      da->writePixel(x + 2, y, color);
      da->writePixel(x + 1, y - 1, color);
      da->writePixel(x, y - 2, color);
      da->writePixel(x - 1, y - 1, color);
      da->writePixel(x - 2, y, color);
      da->writePixel(x - 1, y + 1, color);
      da->writePixel(x, y + 2, color);
      da->writePixel(x + 1, y + 1, color);
    });
  }
  da->endWrite();
}


void AnalogClockElement::_drawHand(uint16_t deg, uint16_t len, uint16_t width, uint32_t color) {
  float a = M_TWOPI * deg / 360;
  float fx = sin(a);
  float fy = -cos(a);

  int16_t x1 = _cx + (fx * len * _radius / 12.0);
  int16_t y1 = _cy + (fy * len * _radius / 12.0);

  _drawLineWidth(_cx, _cy, x1, y1, color, width);
}


// End
