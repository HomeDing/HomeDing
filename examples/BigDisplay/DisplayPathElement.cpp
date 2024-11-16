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

#include <gfxDraw.h>
#include <gfxDrawWidget.h>

// enable TRACE for sending detailed output from this Element
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
  _needLoad = false;

  if (DisplayOutputElement::set(name, value)) {
    // done
    _needLoad = needsDraw;

  } else if (strcmp(name, "path") == 0) {
    _path = value;
    needsDraw = true;
    _needLoad = true;

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

  } else if (strcmp(name, "undraw") == 0) {
    _undraw = _atob(value);

  } else {
    ret = false;
  }  // if

  if (needsDraw && _display) _display->setFlush();

  return (ret);
}  // set()


/// @brief Draw this output element.
void DisplayPathElement::draw() {
  DisplayOutputElement::draw();  // set colors
  // LOGGER_PRINT("draw color=%08lx border=%08lx back=%08lx", _color, _borderColor, _backgroundColor);

  auto fDraw = [&](int16_t x, int16_t y, gfxDraw::ARGB color) {
    _display->writePixel(x, y, color.toColor24());
  };

  auto fGet = [&](int16_t x, int16_t y) -> gfxDraw::ARGB {
    gfxDraw::ARGB col(_display->getPixel(x, y));
    return (col);
  };

  if (!dWidget) {
    dWidget = new gfxDraw::gfxDrawWidget();
    _needLoad = true;

  } else if (_undraw) {
    // LOGGER_PRINT("undraw...");
    // dWidget->undraw(fDraw);
  }

  if (_needLoad) {
    dWidget->setPath(_path.c_str());
    dWidget->setFillColor(gfxDraw::ARGB(_backgroundColor));
    dWidget->setStrokeColor(gfxDraw::ARGB(_borderColor));
  }
  _needLoad = false;

  dWidget->resetTransformation();

  // dObj->setFillGradient(gfxDraw::RED, 4, 6, gfxDraw::YELLOW, 10, 9);
  dWidget->move(-_centerX, -_centerY);
  dWidget->scale(_scale);
  dWidget->rotate(_rotation);
  dWidget->move(_centerX, _centerY);
  dWidget->move(box.x_min + _centerX, box.y_min + _centerY);

  _display->startWrite();
  dWidget->draw(fDraw, _undraw ? fGet : (gfxDraw::fReadPixel) nullptr);
  _display->endWrite();
  
  // the bounding box of dObj is now correct
  // dWidget->bbox;

  TRACE("--done.");
}

// End
