/**
 * @file StateElement.cpp
 * @brief The StateElement implements the configuration for saving the state of a device and its elements.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:see StateElement.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include "StateElement.h"

#define TRACE(...)  // LOGGER_ETRACE(__VA_ARGS__)


/* ===== Static factory function ===== */

/// @brief static factory function to create a new StateElement
/// @return StateElement* created element
Element *StateElement::create() {
  return (new StateElement());
}  // create()


/* ===== Element functions ===== */

/// @brief Set a parameter or property to a new value or start an action.
bool StateElement::set(const char *name, const char *value) {
  TRACE("set(%s, %s)", name, value);

  bool ret = true;

  if (Element::set(name, value)) {
    // all done

  } else if (_stricmp(name, "savedelay") == 0) {
    // save state after a specific time only
    _saveDelay = _scanDuration(value);

  } else if (name == HomeDing::Action::Clear) {
    DeviceState::clear();

  } else {
    ret = false;
  }  // if

  return (ret);
}  // set()


/* ===== State Element functions ===== */


/// @brief trigger saving the current state of the device.
void StateElement::loop() {
  Element::loop();

  if (_saveDelay) {
    unsigned long lc = DeviceState::lastChange();

    if (lc) {
      unsigned long now = millis();  // current (relative) time in msecs.
      if ((lc + _saveDelay) < now) {
        DeviceState::save();
      }
    }
  }
}  // loop()

// Always Register the OTAElement in the ElementRegistry.
bool StateElement::registered =
  ElementRegistry::registerElement("state", StateElement::create);

// End
