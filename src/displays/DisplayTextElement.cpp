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

  } else if (_stricmp(name, "prefix") == 0) {
    _prefix = value;
    needsDraw = true;

  } else if (_stricmp(name, "postfix") == 0) {
    _postfix = value;
    needsDraw = true;

  } else {
    ret = false;
  }  // if

  // set width at least to 1 unit per character...
  if (needsDraw) {
    int16_t right = box.x_min + _prefix.length() + _value.length() + _postfix.length();
    if (right > box.x_max)
      box.x_max = right;
  }

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

    // adjust height and width to the drawn text
    int w = _display->drawText(box.x_min, box.y_min, box.y_max - box.y_min + 1, msg);  // remember width of drawn text
    box.x_max = box.x_min + w - 1;
    box.y_max = box.y_min + _display->getLineHeight() - 1;
  }
}  // draw


// End
