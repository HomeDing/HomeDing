// -----
// TimerElement.cpp - Input Element for creating actions based on a timer.
//
// Copyright (c) by Matthias Hertel, https://www.mathertel.de.
//
// This work is licensed under a BSD style license.
// See https://www.mathertel.de/License.aspx.
// More information on https://www.mathertel.de/Arduino
// -----
// Changelog: see TimerElement.h
// -----

#include <TimerElement.h>
#include <ElementRegistry.h>
#include <Board.h>

/**
 * @brief All timing variables in this class are in seconds.
 * The result of millis() must be divied by 1000 to get seconds.
 */
#define TIMER_UNIT 1000


/**
 * @brief type for looping the timer after cycletime is over.
 */
#define TIMER_TYPE_LOOP 0x01

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
  LOGGER_ETRACE("set(%s, %s)", name, value);
  unsigned long now = millis() / TIMER_UNIT;
  bool ret = true;

  if (_stricmp(name, "type") == 0) {
    if (_stricmp(value, "LOOP") == 0) {
      _type = TIMER_TYPE_LOOP;
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

  } else if (_stricmp(name, "next") == 0) {
    if (_state == 0) {
      _startTime = now - _waitTime;
    } else if (_state == 1) {
      _startTime = now - (_waitTime + _pulseTime);
    } else if (_state == 2) {
      _startTimer();
    }

  } else if (_stricmp(name, "start") == 0) {
    // turn off and do a fresh start
    if (_state == 1) {
      _board->dispatch(_offAction);
    }
    _startTimer();

  } else if (_stricmp(name, "stop") == 0) {
    // turn off and do a fresh start
    if (_state == 1) {
      _board->dispatch(_offAction);
    }
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
  LOGGER_ETRACE("start()");

  if (_cycleTime < _waitTime + _pulseTime) {
    _cycleTime = _waitTime + _pulseTime;
  } // if

  if ((_pulseTime == 0) || (_pulseTime == _cycleTime)) {
    LOGGER_EERR("no meaningful timing");

  } else {
    // start Timer
    Element::start();
    _startTimer();
  } // if
} // start()


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void TimerElement::loop()
{
  unsigned long now = millis() / TIMER_UNIT;

  // time from start in seconds
  uint16_t tfs = now - _startTime;

  if (_state == 0) {
    if (tfs >= _waitTime) {
      // switch state to 1 and submit ON action
      _state = 1;
      _board->dispatch(_onAction);
    } // if

  } else if (_state == 1) {
    if (tfs >= _waitTime + _pulseTime) {
      // switch state to 2 and submit OFF action
      _state = 2;
      _board->dispatch(_offAction);
    } // if

  } else if (_state == 2) {
    if (tfs >= _cycleTime) {
      if (_type == TIMER_TYPE_LOOP) {
        _startTimer();
      } else {
        _stopTimer();
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
  unsigned long now = millis() / TIMER_UNIT;

  Element::pushState(callback);
  callback("state", String(_state).c_str());
  if (_state == 3)
    callback("time", "0");
  else 
    callback("time", String(now - _startTime).c_str());
  callback("value", (_state == 1) ? "1" : "0");
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
  _state = 0;
  _startTime = millis() / TIMER_UNIT;
}

/**
 * @brief Stop the timer.
 */
void TimerElement::_stopTimer()
{
  if (_state == 1) {
    _board->dispatch(_offAction);
  } // if
  _state = 3;
}

// End
