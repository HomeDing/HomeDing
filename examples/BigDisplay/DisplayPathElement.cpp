/**
 * @file DisplayPathElement.cpp
 * @brief Output Element for displaying 7 segment digits on a display.
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
 * Changelog, see DisplayPathElement.h.
 */

#include <Arduino.h>
#include <HomeDing.h>

#include "DisplayPathElement.h"
#include "gfxDraw.h"
#include "gfxDrawObject.h"

#define TRACE(...)  // LOGGER_ETRACE(__VA_ARGS__)

/**
 * @brief static factory function to create a new DisplayPathElement.
 * @return DisplayPathElement* as Element* created element
 */
Element *DisplayPathElement::create() {
  return (new DisplayPathElement());
}  // create()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool DisplayPathElement::set(const char *name, const char *value) {
  TRACE("set %s=%s", name, value);
  bool ret = true;

  if (DisplayOutputElement::set(name, value)) {
    // done

  } else if (strcmp(name, "path") == 0) {
    _path = value;
    needsDraw = true;

  } else if (strcmp(name, "scale") == 0) {
    _scale = _atoi(value);
    needsDraw = true;

  } else if (strcmp(name, "rotation") == 0) {
    _rotation = _atoi(value);
    needsDraw = true;

  } else if (strcmp(name, "center") == 0) {
    _centerX = _atoi(ListUtils::at(value, 0).c_str());
    _centerY = _atoi(ListUtils::at(value, 1).c_str());
    needsDraw = true;

  } else {
    ret = false;
  }  // if

  if (needsDraw && _display) _display->setFlush();

  return (ret);
}  // set()


/// @brief Draw this output element.
void DisplayPathElement::draw() {
  DisplayOutputElement::draw();  // set colors
  TRACE("draw border=%08lx back=%08lx", _borderColor, _backgroundColor);

  gfxDraw::gfxDrawObject *dObj = new gfxDraw::gfxDrawObject(gfxDraw::RGBA(_borderColor), gfxDraw::RGBA(_backgroundColor));

  dObj->setPath(_path.c_str());
  // dObj->setFillGradient(gfxDraw::RED, 4, 6, gfxDraw::YELLOW, 10, 9);
  dObj->move(-_centerX, -_centerY);
  dObj->scale(_scale);
  dObj->rotate(_rotation);
  dObj->move(_centerX, _centerY);
  dObj->move(box.x_min + _centerX, box.y_min + _centerY);

  dObj->draw([&](int16_t x, int16_t y, gfxDraw::RGBA color) {
    // printf("draw %02x %02x %02x %08lx\n", color.Red, color.Green, color.Blue, color.toColor24());
    _display->drawPixel(x, y, color.toColor24());
  });

  TRACE("--done.");
}

// End
