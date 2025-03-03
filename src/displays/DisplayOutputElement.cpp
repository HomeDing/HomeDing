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

  // use default stroke and background color
  _strokeColor = HomeDing::displayConfig.drawColor;
  _backgroundColor = HomeDing::displayConfig.backgroundColor;
}


// ===== Element functions =====

/// @brief Set a parameter or property to a new value or start an action.
bool DisplayOutputElement::set(const char *name, const char *value) {
  TRACE("set %s=%s", name, value);
  int16_t iValue = _atoi(value);
  bool ret = true;

  if (Element::set(name, value)) {
    // done

  } else if (name == HomeDing::Actions::Value) {
    _value = value;

  } else if (name == HomeDing::Actions::Clear) {
    _value.clear();

  } else if (name == HomeDing::Actions::Redraw) {

  } else if (name == HomeDing::Actions::X) {
    int16_t v = iValue;
    _x1 += v - _x0;
    _x0 = v;

  } else if (name == HomeDing::Actions::Y) {
    int16_t v = iValue;
    _y1 += v - _y0;
    _y0 = v;

  } else if (_stricmp(name, "x1") == 0) {
    _x1 = iValue;

  } else if (_stricmp(name, "y1") == 0) {
    _y1 = iValue;

  } else if ((name == HomeDing::Actions::Width) || (_stricmp(name, "w") == 0)) {
    _x1 = _x0 + iValue - 1;

  } else if ((name == HomeDing::Actions::Height) || (_stricmp(name, "h") == 0)) {
    _y1 = _y0 + iValue - 1;

  } else if ((_stricmp(name, "stroke") == 0) || (_stricmp(name, "color") == 0)) {
    _strokeColor = _atoColor(value);

  } else if ((_stricmp(name, "fill") == 0) || (_stricmp(name, "background") == 0)) {
    _backgroundColor = _atoColor(value);

  } else if (name == HomeDing::Actions::FontSize) {
    _fontsize = iValue;

  } else if (_stricmp(name, "align") == 0) {
    _align = (TEXTALIGN)_scanEnum("left,center,right", value);

    // } else if (name == HomeDing::Actions::Border) {
    //   _borderColor = _atoColor(value);

  } else if (!active) {
    // these properties can be used for configuration only.

    if (_stricmp(name, "page") == 0) {
      page = iValue;

    } else {
      ret = false;
    }  // if

  } else {
    ret = false;
  }  // if

  if (ret) {
    box = BoundingBox(_x0, _y0, _x1, _y1);
    needsDraw = true;
    TRACE("box= %d/%d - %d/%d", box.x_min, box.y_min, box.x_max, box.y_max);
  }

  if (needsDraw && HomeDing::displayAdapter) { HomeDing::displayAdapter->setFlush(); }

  return (ret);
}  // set()


/**
 * @brief Activate the DisplayOutputElement.
 */
void DisplayOutputElement::start() {
  TRACE("start()");
  TRACE("colors: #%08x / #%08x", _strokeColor, _backgroundColor);

  // make sure that the page is in the range of valid virtual pages
  if (page > HomeDing::displayAdapter->maxpage) {
    HomeDing::displayAdapter->maxpage = page;
  }

  Element::start();
  needsDraw = true;
  HomeDing::displayAdapter->setFlush();
}  // start()


/// @brief Draw the OutputElement...
void DisplayOutputElement::draw() {
  // LOGGER_ETRACE("draw(%s) page=%d", id, page);
}

/// @brief push the current value of all properties to the callback.
void DisplayOutputElement::pushState(
  std::function<void(const char *pName, const char *eValue)> callback) {
  Element::pushState(callback);
  callback(HomeDing::Actions::Value, _value.c_str());
}  // pushState()

// End
