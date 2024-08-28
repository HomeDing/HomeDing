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

#define TRACE(...)  // LOGGER_ETRACE(__VA_ARGS__)

DisplayOutputElement::DisplayOutputElement() {
  category = CATEGORY::Widget;  // no loop
}

// ===== Element functions =====

/// @brief Set a parameter or property to a new value or start an action.
bool DisplayOutputElement::set(const char *name, const char *value) {
  TRACE("set %s=%s", name, value);
  bool ret = true;

  if (Element::set(name, value)) {
    // done

    // these properties can be changed and redraw will happen

  } else if (name == HomeDing::Action::Value) {
    _value = value;
    needsDraw = true;

  } else if (name == HomeDing::Action::Clear) {
    _value.clear();
    needsDraw = true;

  } else if (name == HomeDing::Action::Redraw) {
    needsDraw = true;

  } else if (name == HomeDing::Action::X) {
    box.x_max -= box.x_min;
    box.x_min = _atoi(value);
    box.x_max += box.x_min;
    needsDraw = true;

  } else if (name == HomeDing::Action::Y) {
    box.y_max -= box.y_min;
    box.y_min = _atoi(value);
    box.y_max += box.y_min;
    needsDraw = true;

  } else if ((name == HomeDing::Action::Width) || (_stricmp(name, "w") == 0)) {
    box.x_max = box.x_min + _atoi(value) - 1;
    needsDraw = true;

  } else if ((name == HomeDing::Action::Height) || (_stricmp(name, "h") == 0) || (_stricmp(name, "fontsize") == 0)) {
    box.y_max = box.y_min + _atoi(value) - 1;
    needsDraw = true;

  } else if (_stricmp(name, "color") == 0) {
    _color = _atoColor(value);
    needsDraw = true;

  } else if (_stricmp(name, "background") == 0) {
    _backgroundColor = _atoColor(value);
    needsDraw = true;

  } else if (name == HomeDing::Action::Border) {
    _borderColor = _atoColor(value);
    needsDraw = true;

  } else if (!active) {
    // these properties can be used for configuration only.

    if (_stricmp(name, "page") == 0) {
      page = _atoi(value);
      needsDraw = true;

    } else {
      ret = false;
    }  // if

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
  callback(HomeDing::Action::Value, _value.c_str());
}  // pushState()

// End
