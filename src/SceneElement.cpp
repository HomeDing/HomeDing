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
  // startupMode = Element::STARTUPMODE::System;
  _delay = 100;       // fast stepping to the next action.
  _currentStep = -1;  // no current step
}


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool SceneElement::set(const char *name, const char *value) {
  TRACE("set %s=%s", name, value);
  bool ret = true;
  int size = _steps.size();

  if (name == HomeDing::Actions::Start) {
    // start the scene at step[0]
    _currentStep = 0;
    _nextStepTime = 1;  // asap.

  } else if (name == HomeDing::Actions::Next) {
    // start next step in in scene
    TRACE("_next cnt=%d, size=%d", _currentStep, size);
    if (_currentStep < size) {
      _currentStep++;
      _nextStepTime = 1;  // asap.
    }

  } else if (name == HomeDing::Actions::Prev) {
    // start previous step in in scene
    TRACE("_next cnt=%d, size=%d", _currentStep, size);
    if (_currentStep > 0) {
      _currentStep--;
      _nextStepTime = 1;  // asap.
    }

  } else if (_stristartswith(name, "steps[")) {
    size_t i;
    String iName;
    _scanIndexParam(name, i, iName);
    _steps.setAt(i, value);

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
  if (_nextStepTime > 0) {
    // some outgoing actions should be sent
    unsigned long now = millis();  // current (relative) time in msecs.
    TRACE("loop( %d, %d)", now, _nextStepTime);

    if ((now >= _nextStepTime) && (HomeDing::Actions::queueIsEmpty())) {
      if ((_currentStep >= 0) && (_currentStep < _steps.size())) {
        String actions = _steps[_currentStep];
        TRACE("send(%d):<%s>", _currentStep, actions.c_str());
        HomeDing::Actions::push(actions);
      }
      _nextStepTime = 0;
      if (_delay >= 0) {
        // send next action after some time
        _currentStep++;
        if (_currentStep < (int)(_steps.size())) {
          _nextStepTime = now + _delay;
        } else {
          // end is reached -> deactivate
          _currentStep = -1;
        }
      }
    }
  }  // if()
}  // loop()


// End
