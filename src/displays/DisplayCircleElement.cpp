/**
 * @file DisplayCircleElement.cpp
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
 * Changelog, see DisplayCircleElement.h.
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <displays/DisplayCircleElement.h>

#include <gfxDraw.h>

#define TRACE(...)  // LOGGER_ETRACE(__VA_ARGS__)

/**
 * @brief static factory function to create a new DisplayCircleElement.
 * @return DisplayCircleElement* as Element* created element
 */
Element *DisplayCircleElement::create() {
  DisplayCircleElement *e = new DisplayCircleElement();
  e->_value = "1";
  return (e);
}  // create()


/// @brief Draw this output element.
void DisplayCircleElement::draw() {
  bool bValue = _atob(_value.c_str());
  TRACE("draw(%d)", bValue);

  HomeDing::strokeColor = _color;
  HomeDing::fillColor = _backgroundColor;

  HomeDing::displayAdapter->startWrite();

  gfxDraw::drawCircle(
    gfxDraw::Point((_x0 + _x1) / 2, (_y0 + _y1) / 2),(_x1 - _x0) / 2,
    HomeDing::stroke, bValue ? HomeDing::fill : nullptr);

  HomeDing::displayAdapter->endWrite();

}  // draw()


// End
