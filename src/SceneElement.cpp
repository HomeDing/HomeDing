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

#define TRACE(...) // LOGGER_EINFO(__VA_ARGS__)


/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new SceneElement
 * @return SceneElement* created element
 */
Element *SceneElement::create() {
  return (new SceneElement());
}  // create()


/* ===== Element functions ===== */

SceneElement::SceneElement() {
  // adjust startupMode when Network (default) is not applicable.
  // startupMode = Element_StartupMode::System;
  _count = -1;
  _delay = 1;  // ms
}


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool SceneElement::set(const char *name, const char *value) {
  bool ret = true;
  int size = _steps.size();

  if (_stricmp(name, "start") == 0) {
    // start the scene
    _count = 0;
    _nextStep = 1;  // asap.

  } else if (_stricmp(name, "step") == 0) {
    // start next step in in scene
    if (_count < size) {
      _nextStep = 1;  // asap.
    } else {
      _count = 0;
      _nextStep = 1;  // asap.
    }

  } else if (_stricmp(name, "next") == 0) {
    // start next step in in scene
    TRACE("_next cnt=%d, size=%d", _count, size);
    if ((_count < 0) || (_count >= size)) {
      _count = 0;
    }
    _nextStep = 1;  // asap.


  } else if (_stristartswith(name, "steps[")) {
    // _steps.push_back(String(value));
    _steps.push_back(value);
    TRACE("_steps cnt=%d", _steps.size());

  } else if (_stricmp(name, "delay") == 0) {
    // delay between executing the steps
    _delay = _scanDuration(value);

  } else {
    ret = Element::set(name, value);
  }  // if

  return (ret);
}  // set()


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void SceneElement::loop() {
  if ((_count >= 0) && (_nextStep > 0)) {
    unsigned long now = millis();  // current (relative) time in msecs.

    TRACE("loop( %ld, %ld)", now, _nextStep);

    if (_nextStep && (now >= _nextStep) && (_board->queueIsEmpty())) {
      // send next scene action
      int size = _steps.size();
      if (_count < size) {
        TRACE("send(%d):<%s>", _count, _steps[_count].c_str());
        _board->dispatch(_steps[_count]);
        _count++;
        _nextStep = ((_delay > 0) ? now + _delay : 0);
      }

      if (_count == size) {
        // all done.
        _count = -1;
      }
    }  // if()
  }    // if()
}  // loop()


// End
