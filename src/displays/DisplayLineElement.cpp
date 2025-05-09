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

#if defined(ESP32)

#include <displays/DisplayLineElement.h>

#include <gfxDraw.h>
#include "DisplayLineElement.h"

#ifndef GFX_TRACE
#define GFX_TRACE(...)  // GFXDRAWTRACE(__VA_ARGS__)
#endif

/**
 * @brief static factory function to create a new DisplayLineElement.
 * @return DisplayLineElement* as Element* created element
 */
Element *DisplayLineElement::create() {
  return (new DisplayLineElement());
}  // create()


void DisplayLineElement::start() {
  GFX_TRACE("start()");
  DisplayOutputElement::start();
  isOpaque = true; // don't draw a background color rectangle.
}


/// @brief Draw the line by using gfxDraw functions.
void DisplayLineElement::draw() {
  GFX_TRACE("draw(%d/%d - %d/%d)\n", _x0, _y0, _x1, _y1);

  HomeDing::displayAdapter->startWrite();
  gfxDraw::drawLine(_x0, _y0, _x1, _y1, HomeDing::writeColor(_strokeColor));
  HomeDing::displayAdapter->endWrite();
}  // draw()

#endif

// End
