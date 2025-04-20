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

  } else if (_stricmp(name, "postfix") == 0) {
    _postfix = value;

  } else {
    ret = false;
  }  // if

  if (ret) { needsDraw = true; }

  return (ret);
}  // set()


/**
 * @brief Activate the DisplayTextElement.
 */
void DisplayTextElement::start() {
  TRACE("start(%p)", HomeDing::displayAdapter);

  if (HomeDing::displayAdapter) {
    DisplayOutputElement::start();

    if (_fontsize > 0 && (_y1 - _y0 + 1 < _fontsize)) {
      // textElement has a minimum fontsize height.
      _y1 = _y0 + _fontsize - 1;
      box = BoundingBox(_x0, _y0, _x1, _y1);

    } else if (_fontsize < 0) {
      _fontsize = _y1 - _y0 + 1;
    }

    TRACE("  box: %d/%d - %d/%d", box.x_min, box.y_min, box.x_max, box.y_max);
    TRACE("  fontsize = %d", _fontsize);
  }
}  // start()



/// @brief send current text to display
void DisplayTextElement::draw() {
  TRACE("draw(%p)", HomeDing::displayAdapter);

  DisplayOutputElement::draw();

  String msg(_prefix);
  msg.concat(_value);
  msg.concat(_postfix);

  TRACE("draw %d,%d h:%d \"%s\"", box.x_min, box.y_min, _fontsize, msg.c_str());

  BoundingBox txtBox = HomeDing::displayAdapter->textBox(_fontsize, msg.c_str());
  txtBox.shift(box.x_min, box.y_min);

  // adjust bounding box to include the box used for drawing the text
  box.extend(txtBox);
  _x1 = box.x_max;
  _y1 = box.y_max;

  TRACE("  box: %d/%d - %d/%d", box.x_min, box.y_min, box.x_max, box.y_max);

  HomeDing::displayAdapter->drawText(box.x_min, box.y_min, _fontsize, msg.c_str(), _strokeColor);
}  // draw


// End
