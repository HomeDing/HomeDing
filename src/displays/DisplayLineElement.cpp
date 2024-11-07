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
  TRACE("draw(%d/%d - %d/%d)\n", _x0, _y0, _x1, _y1);

  if ((_color != RGB_UNDEFINED) && (_color != RGB_TRANSPARENT)) {
    HomeDing::strokeColor = _color;
    HomeDing::displayAdapter->startWrite();
    gfxDraw::drawLine(_x0, _y0, _x1, _y1, HomeDing::stroke);
    HomeDing::displayAdapter->endWrite();
  }
}  // draw()



// End
