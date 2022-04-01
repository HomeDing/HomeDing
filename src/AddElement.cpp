/**
 * @file AddElement.cpp
 * @brief Element that adds given input values.
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
 * Changelog:see AddElement.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <AddElement.h>


/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new AndElement
 * @return AddElement* created element
 */
Element *AddElement::create() {
  return (new AddElement());
} // create()


/* ===== Element functions ===== */

void AddElement::start() {
  CalcElement::start();
  _type = DATATYPE::INTEGER;
}


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void AddElement::_calc() {
  int newValue = 0;

  for (int n = 0; n < _inputs; n++) {
    newValue += atoi(_inStringValues[n].c_str());
  }
  _value = String(newValue);
  CalcElement::_calc();
} // _calc()


// End
