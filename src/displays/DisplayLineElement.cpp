/**
 * @file DisplayLineElement.cpp
 * @brief Output Element for controlling a binary output on a display.
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
 * Changelog, see DisplayLineElement.h.
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <displays/DisplayLineElement.h>

#include <gfxDraw.h>

#define TRACE(...) // LOGGER_ETRACE(__VA_ARGS__)

 /**
 * @brief static factory function to create a new DisplayLineElement.
 * @return DisplayLineElement* as Element* created element
 */
Element *DisplayLineElement::create() {
  return (new DisplayLineElement());
}  // create()


/// @brief Draw the line by using gfxDraw functions.
void DisplayLineElement::draw() {
  TRACE("drawLine(%d/%d - %d/%d #%08x)\n", _x0, _y0, _x1, _y1, _color);
  DisplayAdapter *d = _display;
  uint32_t color = _color;

  if ((_color != RGB_UNDEFINED) && (_color != RGB_TRANSPARENT)) {
    _display->startWrite();
    gfxDraw::drawLine(_x0, _y0, _x1, _y1, [d, color](int16_t x, int16_t y) {
      if (d->displayBox.contains(x, y)) {
        d->writePixel(x, y, color);
      }
    });
    _display->endWrite();
  }
}  // draw()



// End
