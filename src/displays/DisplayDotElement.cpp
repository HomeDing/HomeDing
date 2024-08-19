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
  TRACE("draw()");
  bool bValue = _atob(_value.c_str());
  _display->drawCircle(box, _borderColor, bValue ? _backgroundColor : RGB_TRANSPARENT);
}  // draw()


// End
