/**
 * @file DisplayDotElement.cpp
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
 * Changelog, see DisplayDotElement.h.
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <displays/DisplayDotElement.h>

#include <gfxDraw.h>

#define TRACE(...)  // LOGGER_ETRACE(__VA_ARGS__)

/**
 * @brief static factory function to create a new DisplayDotElement.
 * @return DisplayDotElement* as Element* created element
 */
Element *DisplayDotElement::create() {
  DisplayDotElement *e = new DisplayDotElement();
  e->_value = "1";
  return (e);
}  // create()


/// @brief Draw this output element.
void DisplayDotElement::draw() {
  TRACE("draw(%d/%d - %d/%d #%08x)\n", _x0, _y0, _x1, _y1);

  bool bValue = _atob(_value.c_str());

#if 0
  _display->drawCircle(box, _borderColor, bValue ? _backgroundColor : RGB_TRANSPARENT);

#else
  HomeDing::strokeColor = _borderColor;
  HomeDing::fillColor = _backgroundColor;

  HomeDing::displayAdapter->startWrite();

  gfxDraw::drawCircle(
    gfxDraw::Point((_x0 + _x1) / 2, (_y0 + _y1) / 2),
    (_x1 - _x0) / 2,
    HomeDing::stroke,
    HomeDing::fill);

  HomeDing::displayAdapter->endWrite();
#endif

}  // draw()


// End
