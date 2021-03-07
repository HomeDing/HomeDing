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

/**
 * @brief static factory function to create a new TimerElement
 * @return TimerElement* created element
 */
Element *TimerElement::create()
{
  return (new TimerElement());
} // create()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool TimerElement::set(const char *name, const char *value)
{
  unsigned long now = _board->getSeconds();
  bool ret = true;

  if (_stricmp(name, "type") == 0) {
    if (_stricmp(value, "loop") == 0) {
      _type = TIMER_TYPE_LOOP;
    } else if (_stricmp(value, "once") == 0) {
      _type = TIMER_TYPE_ONCE; // wait for action to start, no auto re-starting
    } else {
      LOGGER_EERR("unknown type");
      ret = false;
    }

  } else if (_stricmp(name, "cycletime") == 0) {
    _cycleTime = _atotime(value);

  } else if (_stricmp(name, "waittime") == 0) {
    _waitTime = _atotime(value);

  } else if (_stricmp(name, "pulsetime") == 0) {
    _pulseTime = _atotime(value);

  } else if (_stricmp(name, "onon") == 0) {
    _onAction = value;

  } else if (_stricmp(name, "onoff") == 0) {
    _offAction = value;

  } else if (_stricmp(name, "onend") == 0) {
    _endAction = value;

  } else if (_stricmp(name, ACTION_ONVALUE) == 0) {
    _valueAction = value;

  } else if (_stricmp(name, "next") == 0) {
    if (_state == TIMERSTATE_WAIT) {
      _startTime = now - _waitTime;
    } else if (_state == TIMERSTATE_PULSE) {
      _startTime = now - (_waitTime + _pulseTime);
    } else if (_state == TIMERSTATE_CYCLE) {
      _startTimer();
    }

  } else if (_stricmp(name, "start") == 0) {
    // turn off and do a fresh start
    if (_state == TIMERSTATE_PULSE) {
      _board->dispatch(_offAction);
    }
    _startTimer();

  } else if (_stricmp(name, "stop") == 0) {
    // turn off 
    _stopTimer();

  } else {
    ret = Element::set(name, value);
  } // if

  return (ret);
} // set()


/**
 * @brief Activate the TimerElement.
 */
void TimerElement::start()
{
  // TRACE("start()");

  if (_cycleTime < _waitTime + _pulseTime) {
    _cycleTime = _waitTime + _pulseTime;
  } // if

  Element::start();

  if (_type == TIMER_TYPE_LOOP) {
    // start Timer automatically
    _startTimer();
  } // if
} // start()


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void TimerElement::loop()
{
  if (_state != TIMERSTATE_ENDED) {
    // time from start in seconds
    uint16_t tfs = _board->getSeconds() - _startTime;

    if (_state == TIMERSTATE_WAIT) {
      if (tfs >= _waitTime) {
        // switch state to 1 and submit ON action
        _state = TIMERSTATE_PULSE;
        _board->dispatch(_onAction);
        _board->dispatch(_valueAction, "1");
      } // if

    } else if (_state == TIMERSTATE_PULSE) {
      if (tfs >= _waitTime + _pulseTime) {
        // switch state to 2 and submit OFF action
        _state = TIMERSTATE_CYCLE;
        _board->dispatch(_offAction);
        _board->dispatch(_valueAction, "0");
      } // if

    } else if (_state == TIMERSTATE_CYCLE) {
      if (tfs >= _cycleTime) {
        _board->dispatch(_endAction);
        if (_type == TIMER_TYPE_LOOP) {
          _startTimer();
        } else {
          _stopTimer();
        } // if
      } // if
    } // if
  } // if
} // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void TimerElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  unsigned long now = _board->getSeconds();

  Element::pushState(callback);
  callback("state", String(_state).c_str());
  if (_state == TIMERSTATE_ENDED)
    callback("time", "0");
  else
    callback("time", String(now - _startTime).c_str());
  callback(PROP_VALUE, (_state == TIMERSTATE_PULSE) ? "1" : "0");
} // pushState()


/**
 * @brief stop all activities of the timer and go inactive.
 */
void TimerElement::term()
{
  _stopTimer();
  Element::term();
} // term()


/**
 * @brief Start / Restart timer
 */
void TimerElement::_startTimer()
{
  _state = TIMERSTATE_WAIT;
  _startTime = _board->getSeconds();
}

/**
 * @brief Stop the timer.
 */
void TimerElement::_stopTimer()
{
  if (_state == TIMERSTATE_PULSE) {
    _board->dispatch(_offAction);
  } // if
  _state = TIMERSTATE_ENDED;
}

// End
