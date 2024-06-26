/**
 * @file AndElement.cpp
 * @brief Element that combines boolean input values using the AND and optional NOT operator and sends actions.
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
 * Changelog:see AndElement.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <AndElement.h>

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new AndElement
 * @return AndElement* created element
 */
Element *AndElement::create() {
  AndElement *el = new AndElement();
  el->_type = DATATYPE::BOOLEAN;
  return (el);
} // create()


/* ===== Element functions ===== */

/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void AndElement::_calc() {
  bool newValue = (_inputs > 0); // out = false when no inbound value is given.

  // must not continue looping when newValue == false.
  for (int n = 0; (newValue) && (n < _inputs); n++) {
    newValue = _atob(_inStringValues[n].c_str());
  }

  if (_invert) {
    newValue = !newValue;
  }
  _value = newValue;
} // loop()


// End
