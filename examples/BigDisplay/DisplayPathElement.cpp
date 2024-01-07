/**
 * @file DisplayPathElement.cpp
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
 * Changelog, see DisplayPathElement.h.
 */

#include <Arduino.h>
#include <HomeDing.h>

#include "DisplayPathElement.h"
#include "drawAlgo.h"

#define TRACE(...) LOGGER_ETRACE(__VA_ARGS__)

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

  } else if (_stricmp(name, "path") == 0) {
    _path = value;
    _needredraw = true;

  } else {
    ret = false;
  }  // if
  return (ret);
}  // set()


/// @brief Draw this output element.
void DisplayPathElement::draw() {
  TRACE("draw");
  DisplayOutputElement::draw(); // set colors

  DrawAlgo *da = new DrawAlgo();

  // da->rect(
  //   _x, _y, 100, 40,
  //   [&](int16_t x, int16_t y) {
  //     _display->drawPixel(x, y, _borderColor);
  //   },
  //   [&](int16_t x, int16_t y) {
  //     _display->drawPixel(x, y, _backgroundColor);
  //   },
  //   2);

  da->path(
    _path.c_str(), _x, _y,
    [&](int16_t x, int16_t y) {
      _display->drawPixel(x, y, _borderColor);
    },
    [&](int16_t x, int16_t y) {
      _display->drawPixel(x, y, _backgroundColor);
    });
}

/**
 * @brief push the current value of all properties to the callback.
 */
void DisplayPathElement::pushState(
  std::function<void(const char *pName, const char *eValue)> callback) {
  Element::pushState(callback);
}  // pushState()

// End
