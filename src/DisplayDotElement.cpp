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

#include <DisplayDotElement.h>

#define TRACE(...)  // LOGGER_ETRACE(__VA_ARGS__)

/**
 * @brief static factory function to create a new DisplayDotElement.
 * @return DisplayDotElement* as Element* created element
 */
Element *DisplayDotElement::create() {
  return (new DisplayDotElement());
}  // create()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool DisplayDotElement::set(const char *name, const char *value) {
  TRACE("set %s=%s", name, value);
  bool ret = true;

  if (DisplayOutputElement::set(name, value)) {
    // done

  } else if (_stricmp(name, "value") == 0) {
    _value = _atob(value);
    _needredraw = true;

  } else if (_stricmp(name, "clear") == 0) {
    _value = false;
    _needredraw = true;

  } else {
    ret = false;
  }  // if

  return (ret);
}  // set()


/**
 * @brief Draw this output element.
 *
 */
void DisplayDotElement::draw() {
  DisplayOutputElement::draw();
  _display->clear(_x, _y, _h, _h);
  _display->drawDot(_x, _y, _h, _value);
}

/**
 * @brief push the current value of all properties to the callback.
 */
void DisplayDotElement::pushState(
  std::function<void(const char *pName, const char *eValue)> callback) {
  Element::pushState(callback);
  callback("value", _value ? "1" : "0");
}  // pushState()

// End
