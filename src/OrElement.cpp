/**
 * @file OrElement.cpp
 * @brief Element that combines boolean input values using the OR and optional NOT operator and sends actions.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license.
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:see OrElement.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <OrElement.h>

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new OrElement
 * @return OrElement* created element
 */
Element *OrElement::create() {
  OrElement *el = new OrElement();
  el->_type = DATATYPE::BOOLEAN;
  return (el);
} // create()


/* ===== Element functions ===== */

/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void OrElement::_calc() {
  bool newValue = false; // out = false when no inbound value is given.

  // must not continue looping when newValue == true.
  for (int n = 0; (! newValue) && (n < _inputs); n++) {
    newValue = _atob(_inStringValues[n].c_str());
  }

  if (_invert) {
    newValue = !newValue;
  }
  _value = newValue;
} // loop()


// End
