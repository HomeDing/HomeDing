/**
 * @file SceneElement.cpp
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:see SceneElement.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include "SceneElement.h"

#define TRACE(...) // LOGGER_ETRACE(__VA_ARGS__)


/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new SceneElement
 * @return SceneElement* created element
 */
Element *SceneElement::create() {
  return (new SceneElement());
} // create()


/* ===== Element functions ===== */

SceneElement::SceneElement() {
  // adjust startupMode when Network (default) is not applicable.
  // startupMode = Element_StartupMode::System;
  _count = -1;
  _delay = 0;
}


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool SceneElement::set(const char *name, const char *value) {
  bool ret = true;

  if (_stricmp(name, "start") == 0) {
    // start the scene
    _count = 0;
    _nextStep = 0; // asap.

  } else if (_stristartswith(name, "steps[")) {
    size_t index = _atoi(name + 6); // number starts after "steps["

    if (index >= _steps.size()) {
      // set default values for new index
      _steps.resize(index + 1);
      _steps[index] = value;
      TRACE("new(%d):<%s>", index, value);
    } // if

  } else if (_stricmp(name, "delay") == 0) {
    // delay between executing the steps
    _delay = _scanDuration(value);

  } else {
    ret = Element::set(name, value);
  } // if

  return (ret);
} // set()


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void SceneElement::loop() {
  if (_count >= 0) {
    unsigned long now = millis(); // current (relative) time in msecs.

    if ((now >= _nextStep) && (_board->queueIsEmpty())) {
      // send next scene action
      int size = _steps.size();
      if (_count < size) {
        TRACE("send(%d):<%s>", _count, _steps[_count].c_str());
        _board->dispatch(_steps[_count]);
        _count++;
        _nextStep = now + _delay;
      }

      if (_count == size) {
        // all done.
        _count = -1;
      }
    } // if()
  } // if()
} // loop()


// End
