/**
 * @file DisplayRectElement.cpp
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
 * Changelog, see DisplayRectElement.h.
 */

#include <Arduino.h>
#include <HomeDing.h>

#if defined(ESP32)

#include <displays/DisplayRectElement.h>

#include <gfxDraw.h>
#include "DisplayRectElement.h"

#define TRACE(...)  // LOGGER_ETRACE(__VA_ARGS__)

/// @brief static factory function to create a new DisplayRectElement.
/// @return DisplayRectElement* as Element* created element.
Element *DisplayRectElement::create() {
  DisplayRectElement *e = new DisplayRectElement();
  e->_value = "1";
  return (e);
}  // create()


/// @brief Draw the rectangle.
void DisplayRectElement::draw() {
  draw(_atob(_value.c_str()));
}  // draw()


void DisplayRectElement::draw(bool useBackground) {
  TRACE("draw(%d)", useBackground);

  HomeDing::displayAdapter->startWrite();
  gfxDraw::drawRect(
    box.x_min, box.y_min, box.width(), box.height(),
    HomeDing::writeColor(_strokeColor),
    useBackground ? HomeDing::writeColor(_backgroundColor) : nullptr);
  HomeDing::displayAdapter->endWrite();
}
#endif

// End
