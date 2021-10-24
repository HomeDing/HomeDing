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
  return (new OrElement());
} // create()


/* ===== Element functions ===== */

void OrElement::start() {
  CalcElement::start();
  _type = DATATYPE::BOOLEAN;
}

/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void OrElement::_calc() {
  bool newValue = (_inputs > 0); // out = false when no inbound value is given.


  for (int n = 0; n < _inputs; n++) {
    newValue = (newValue || _atob(_inStringValues[n].c_str()));
  }

  if (_invert) {
    newValue = !newValue;
  }
  _value = String(newValue);
  CalcElement::_calc();
} // loop()


// End
