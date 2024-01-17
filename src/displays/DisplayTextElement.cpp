/**
 * @file DisplayTextElement.cpp
 * @brief Output Element for controlling a text output on a display.
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
 * Changelog, see DisplayTextElement.h.
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <displays/DisplayTextElement.h>

#define TRACE(...)  // LOGGER_ETRACE(__VA_ARGS__)

/**
 * @brief static factory function to create a new DisplayTextElement.
 * @return DisplayTextElement* as Element* created element
 */
Element *DisplayTextElement::create() {
  return (new DisplayTextElement());
}  // create()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool DisplayTextElement::set(const char *name, const char *value) {
  bool ret = true;

  if (DisplayOutputElement::set(name, value)) {
    // done

  } else if (_stricmp(name, "value") == 0) {
    _value = value;
    _needredraw = true;

  } else if (_stricmp(name, "clear") == 0) {
    _value.clear();
    _needredraw = true;

  } else if (_stricmp(name, "prefix") == 0) {
    _prefix = value;

  } else if (_stricmp(name, "postfix") == 0) {
    _postfix = value;

  } else {
    ret = false;
  }  // if

  return (ret);
}  // set()


/**
 * @brief send current text to display
 */
void DisplayTextElement::draw() {
  DisplayOutputElement::draw();
  if (_display) {
    String msg(_prefix);
    msg.concat(_value);
    msg.concat(_postfix);

    if (box_w && box_h) {
      _display->clear(box_x, box_y, box_w, box_h);
    }
    box_w = _display->drawText(box_x, box_y, box_h, msg);  // remember width of drawn text
    box_h = _display->getLineHeight();            // adjust height
  }
}  // draw


/**
 * @brief push the current value of all properties to the callback.
 */
void DisplayTextElement::pushState(
  std::function<void(const char *pName, const char *eValue)> callback) {
  DisplayOutputElement::pushState(callback);
  callback(PROP_VALUE, _value.c_str());
}  // pushState()

// End
