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

#define TRACE(...)  // LOGGER_ETRACE(__VA_ARGS__)


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
  _delay = -1;  // manual stepping
  _step = -1;   // no current step
}


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool SceneElement::set(const char *name, const char *value) {
  bool ret = true;
  int size = _steps.size();

  if (_stricmp(name, "start") == 0) {
    // start the scene at step[0]
    _step = 0;
    _nextStep = 1;  // asap.

  } else if (_stricmp(name, "next") == 0) {
    // start next step in in scene
    TRACE("_next cnt=%d, size=%d", _step, size);
    if (_step < size) {
      _step++;
      _nextStep = 1;  // asap.
    }

  } else if (_stricmp(name, "prev") == 0) {
    // start previous step in in scene
    TRACE("_next cnt=%d, size=%d", _step, size);
    if (_step > 0) {
      _step--;
      _nextStep = 1;  // asap.
    }

  } else if (_stristartswith(name, "steps[")) {
    // _steps.push_back(String(value));
    _steps.push_back(value);
    TRACE("_steps.size=%d", _steps.size());

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
  if (_nextStep > 0) {
    // some outgoing actions should be sent
    unsigned long now = millis();  // current (relative) time in msecs.
    TRACE("loop( %ld, %ld)", now, _nextStep);

    if ((now >= _nextStep) && (_board->queueIsEmpty())) {
      TRACE("send(%d):<%s>", _step, _steps[_step].c_str());
      _board->dispatch(_steps[_step]);
      _nextStep = 0;

      if (_delay >= 0) {
        // send next action after some time
        _step++;
        if (_step < _steps.size()) {
          _nextStep = now + _delay;
        } else {
          // end is reached -> deactivate
          _step = -1;
        }
      }
    }
  }  // if()
}  // loop()


// End
