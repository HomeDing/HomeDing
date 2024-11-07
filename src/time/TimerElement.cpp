// -----
// TimerElement.cpp - Input Element for creating actions based on a timer.
//
// Copyright (c) by Matthias Hertel, https://www.mathertel.de.
//
// This work is licensed under a BSD style license.
// See https://www.mathertel.de/License.aspx.
// More information on https://www.mathertel.de/Arduino
// -----
// Changelog: see time/TimerElement.h
// -----

#include <Arduino.h>
#include <HomeDing.h>

#include <time/TimerElement.h>

#define TRACE(...)  // LOGGER_ETRACE(__VA_ARGS__)

/**
 * @brief static factory function to create a new TimerElement
 * @return TimerElement* created element
 */
Element *TimerElement::create() {
  return (new TimerElement());
}  // create()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool TimerElement::set(const char *name, const char *value) {
  unsigned long now = millis();
  bool ret = true;
  // TRACE("set %s=%s", name, value);

  if (name == HomeDing::Actions::Mode) {
    if (_stricmp(value, "off") == 0) {
      _mode = Mode::OFF;
    } else if (_stricmp(value, "on") == 0) {
      _mode = Mode::ON;
    } else if (_stricmp(value, "timer") == 0) {
      _mode = Mode::TIMER;
    }

  } else if (_stricmp(name, "restart") == 0) {
    _restart = _atob(value);

  } else if (_stricmp(name, "cycletime") == 0) {
    _cycleTime = _scanDuration(value);

  } else if (_stricmp(name, "waittime") == 0) {
    _waitTime = _scanDuration(value);

  } else if (_stricmp(name, "pulsetime") == 0) {
    _pulseTime = _scanDuration(value);

  } else if (_stricmp(name, "start") == 0) {
    _mode = Mode::TIMER;
    _startTime = now;

  } else if (_stricmp(name, "stop") == 0) {
    if (_mode == Mode::TIMER) {
      HomeDing::Actions::push(_endAction);
    }
    _mode = Mode::OFF;

  } else if (_stricmp(name, "next") == 0) {
    if (_mode == Mode::TIMER) {
      // time from start in milliseconds
      unsigned long tfs = now - _startTime;

      if (tfs < _waitTime) {
        // skip wait phase
        _startTime = now - _waitTime;

      } else if (tfs < _waitTime + _pulseTime) {
        // skip pulse phase
        _startTime = now - (_waitTime + _pulseTime);

      } else {
        // restart
        _startTime = now;
      }
    }  // if TIMER

  } else if (_stricmp(name, "onon") == 0) {
    _onAction = value;

  } else if (_stricmp(name, "onoff") == 0) {
    _offAction = value;

  } else if (_stricmp(name, "onend") == 0) {
    _endAction = value;

  } else if (name == HomeDing::Actions::OnValue) {
    _valueAction = value;

  } else {
    ret = Element::set(name, value);
  }  // if

  _forceSendActions = true;
  return (ret);
}  // set()


/**
 * @brief Activate the TimerElement.
 */
void TimerElement::start() {
  // TRACE("start()");

  if (_cycleTime < _waitTime + _pulseTime) {
    _cycleTime = _waitTime + _pulseTime;
  }  // if

  Element::start();
  if ((_mode == Mode::TIMER) && (_restart)) {
    // start Timer automatically
    _startTime = millis();
  } else {
    _mode = Mode::OFF;
  }  // if
}  // start()


/**
 * @brief Give some processing time to the Element to check for next actions.
 * will never change the mode.
 */
void TimerElement::loop() {
  bool newValue = _value;

  if (_mode == Mode::ON) {
    newValue = true;

  } else if (_mode == Mode::OFF) {
    newValue = false;

  } else if (_mode == Mode::TIMER) {
    unsigned long now = _board->nowMillis;

    // time from start in milliseconds
    unsigned long tfs = now - _startTime;

    if (tfs < _waitTime) {
      newValue = false;

    } else if (tfs < _waitTime + _pulseTime) {
      newValue = true;

    } else if (tfs < _cycleTime) {
      newValue = false;

    } else {
      newValue = false;
      if (_restart) {
        // and update in next loop()
        _startTime = now;
      } else {
        _mode = Mode::OFF;
      }
    }
  }  // if

  if (! _forceSendActions && (newValue == _value)) {
    // no need to send an action.

  } else if (newValue) {
    HomeDing::Actions::push(_onAction);
    HomeDing::Actions::push(_valueAction, "1");

  } else {
    HomeDing::Actions::push(_offAction);
    HomeDing::Actions::push(_valueAction, "0");
  }  // if
  _forceSendActions = false;
  _value = newValue;
}  // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void TimerElement::pushState(
  std::function<void(const char *pName, const char *eValue)> callback) {
  unsigned long now = millis();

  Element::pushState(callback);
  callback("mode", _mode == Mode::TIMER ? "timer"
                   : _mode == Mode::ON  ? "on"
                                        : "off");
  if (_mode != Mode::TIMER) {
    callback("time", "0");
  } else {
    callback("time", _printInteger((now - _startTime) / 1000));
  }
  callback(HomeDing::Actions::Value, _value ? "1" : "0");
}  // pushState()


/**
 * @brief stop all activities of the timer and go inactive.
 */
void TimerElement::term() {
  if (_mode == Mode::TIMER) {
    HomeDing::Actions::push(_endAction, 0);
  }
  _mode = Mode::OFF;

  if (_value) {
    HomeDing::Actions::push(_offAction, 0);
    HomeDing::Actions::push(_valueAction, 0);
    _value = false;
  }

  Element::term();
}  // term()


// End
