/**
 * @file OrElement.cpp
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

/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void OrElement::loop() {
  if (_needRecalc) {
    bool newOut = false; // out = false when no inbound value is given.

    for (int n = 0; n < _inputs; n++) {
      newOut = (newOut || _value[n]);
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
