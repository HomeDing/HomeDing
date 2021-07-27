/**
 * @file AndElement.cpp
 * @brief Logical Element that combines boolean input values using the AND and optional NOT operator and sends actions.
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
  return (new AndElement());
} // create()


/* ===== Element functions ===== */

/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void AndElement::loop() {
  if (_needRecalc) {
    bool newOut = (_inputs > 0); // out = false when no inbound value is given.

    for (int n = 0; n < _inputs; n++) {
      newOut = (newOut && _value[n]);
    }

    if (_invert) {
      newOut = !newOut;
    }

    if (newOut != _outValue) {
      _board->dispatch(_valueAction, newOut);
      _outValue = newOut;
    }

    _needRecalc = false;
  } // if
  LogicElement::loop();
} // loop()


// End
