/**
 * @file displays/DisplayOutputElement.cpp
 * @brief Base Output Element for a display.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 *
 * More information on https://www.mathertel.de/Arduino.
 *
 * Changelog, see DisplayOutputElement.h.
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <displays/DisplayOutputElement.h>
#include "DisplayOutputElement.h"

#define TRACE(...) // LOGGER_ETRACE(__VA_ARGS__)

DisplayOutputElement::DisplayOutputElement() {
  category = CATEGORY::Widget;
}

// ===== Element functions =====

/// @brief Set a parameter or property to a new value or start an action.
bool DisplayOutputElement::set(const char *name, const char *value) {
  TRACE("set %s=%s", name, value);
  bool ret = true;

  if (Element::set(name, value)) {
    // done

    // these properties can be changed and redraw will happen

  } else if (_stricmp(name, "value") == 0) {
    _value = value;
    needsDraw = true;

  } else if (_stricmp(name, "clear") == 0) {
    _value.clear();
    needsDraw = true;

  } else if (_stricmp(name, "redraw") == 0) {
    needsDraw = true;

  } else if (_stricmp(name, "x") == 0) {
    _x = box.x_min = _atoi(value);
    box.x_max = _x + _w - 1;
    needsDraw = true;

  } else if (_stricmp(name, "y") == 0) {
    _y = box.y_min = _atoi(value);
    box.y_max = _y + _h - 1;
    needsDraw = true;

  } else if ((_stricmp(name, "w") == 0) || (_stricmp(name, "width") == 0)) {
    _w = _atoi(value);
    box.x_max = _x + _w - 1;
    needsDraw = true;

  } else if ((_stricmp(name, "h") == 0) || (_stricmp(name, "height") == 0) || (_stricmp(name, "fontsize") == 0)) {
    _h = _atoi(value);
    box.y_max = _y + _h - 1;
    needsDraw = true;

  } else if (_stricmp(name, "color") == 0) {
    _color = _atoColor(value);
    needsDraw = true;

  } else if (_stricmp(name, "background") == 0) {
    _backgroundColor = _atoColor(value);
    needsDraw = true;

  } else if (_stricmp(name, "border") == 0) {
    _borderColor = _atoColor(value);
    needsDraw = true;

    // these properties can be used for configuration only.

  } else if (_stricmp(name, "page") == 0) {
    page = _atoi(value);
    needsDraw = true;

  } else {
    ret = false;
  }  // if

  if (needsDraw && _display) _display->setFlush();

  return (ret);
}  // set()


/**
 * @brief Activate the DisplayOutputElement.
 */
void DisplayOutputElement::start() {
  TRACE("start()");
  _display = _board->display;
  if (_display) {

    if (_color == RGB_UNDEFINED)
      _color = _display->getColor();
    if (_backgroundColor == RGB_UNDEFINED)
      _backgroundColor = _display->getBackgroundColor();
    if (_borderColor == RGB_UNDEFINED)
      _borderColor = _display->getBorderColor();
    TRACE("colors: #%08x / #%08x / #%08x", _color, _backgroundColor, _borderColor);

    if (page > _display->maxpage) {
      _display->maxpage = page;
    }

    Element::start();
    needsDraw = true;
    if (_display) _display->setFlush();
  }  // if
}  // start()


/**
 * @brief check the state of the DHT values and eventually create actions.
 */
void DisplayOutputElement::loop() {
  if (needsDraw && active) {
    if (_display && (!_display->deferDrawing()) && (_display->page == page)) {
      // need erase background
      auto bCol = _display->getBackgroundColor();
      _display->drawRectangle(box, RGB_TRANSPARENT, _display->getBackgroundColor());

      // draw immediately (no overlapping elements)
      draw();
      needsDraw = false;
    }
  }  // if
}  // loop()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
void DisplayOutputElement::draw() {
  // LOGGER_ETRACE("draw(%s) page=%d", id, page);
  _display->setColor(_color);
  _display->setBackgroundColor(_backgroundColor);
  _display->setBorderColor(_borderColor);
}

/// @brief push the current value of all properties to the callback.
void DisplayOutputElement::pushState(
  std::function<void(const char *pName, const char *eValue)> callback) {
  Element::pushState(callback);
  callback("value", _value.c_str());
}  // pushState()

// End
