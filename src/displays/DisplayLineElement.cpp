/**
 * @file DisplayLineElement.cpp
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
 * Changelog, see DisplayLineElement.h.
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <displays/DisplayLineElement.h>

/**
 * @brief static factory function to create a new DisplayLineElement.
 * @return DisplayLineElement* as Element* created element
 */
Element *DisplayLineElement::create() {
  return (new DisplayLineElement());
}  // create()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool DisplayLineElement::set(const char *name, const char *value) {
  bool ret = true;

  if (DisplayOutputElement::set(name, value)) {
    // done
  } else if (_stricmp(name, "x1") == 0) {
    _x1 = _atoi(value);

  } else if (_stricmp(name, "y1") == 0) {
    _y1 = _atoi(value);

  } else {
    ret = false;
  }  // if

  return (ret);
}  // set()


/**
 * @brief Draw this output element.
 *
 */
void DisplayLineElement::draw() {
  DisplayOutputElement::draw();
  _display->drawLine(box.x_min, box.y_min, _x1, _y1);
}


// End
