/**
 * @file ScheduleElement.cpp
 * @brief The ScheduleElement creates actions based on the time of day.
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
 * Changelog:see time/ScheduleElement.h
 */


#include <Arduino.h>
#include <HomeDing.h>

#include <time/ScheduleElement.h>

/* ===== Define local constants and often used strings ===== */

// use a time > 24h to indicate no time was configured.
#define TIME_T_NOT (48 * 60 * 60)


/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new ScheduleElement
 * @return ScheduleElement* created element
 */
Element *ScheduleElement::create()
{
  return (new ScheduleElement());
} // create()


/* ===== Element functions ===== */

ScheduleElement::ScheduleElement()
{
  startupMode = Element_StartupMode::Time;
}

/**
 * @brief initialize the common functionality of all element objects.
 */
void ScheduleElement::init(Board *board)
{
  Element::init(board);

  _startTime = TIME_T_NOT;
  _endTime = TIME_T_NOT;
} // init()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool ScheduleElement::set(const char *name, const char *value)
{
  bool ret = true;

  if (_stricmp(name, "ontime") == 0) {
    _startTime = _atotime(value);

  } else if (_stricmp(name, "offtime") == 0) {
    _endTime = _atotime(value);

  } else if (_stricmp(name, "onon") == 0) {
    _onAction = value;

  } else if (_stricmp(name, "onoff") == 0) {
    _offAction = value;

  } else if (_stricmp(name, "mode") == 0) {
    if (_stricmp(value, "off") == 0) {
      _mode = Mode::OFF;
    } else if (_stricmp(value, "on") == 0) {
      _mode = Mode::ON;
    } else if (_stricmp(value, "timer") == 0) {
      _mode = Mode::TIMER;
    }

  } else if (_stricmp(name, ACTION_ONVALUE) == 0) {
    _valueAction = value;

  } else {
    ret = Element::set(name, value);
  } // if

  return (ret);
} // set()


/**
 * @brief Activate the ScheduleElement.
 */
void ScheduleElement::start()
{
  // Verify parameters
  _init = false;

  if ((_startTime == TIME_T_NOT) && (_endTime == TIME_T_NOT)) {
    LOGGER_EERR("no time set");
  } else {
    Element::start();
  } // if
} // start()


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void ScheduleElement::loop()
{
  time_t ct = _board->getTimeOfDay();
  bool newValue = _value;

  if (_mode == Mode::ON) {
    newValue = true;

  } else if (_mode == Mode::OFF) {
    newValue = false;

  } else if (ct > 0) {
    // There is a local time available.

    // find the current value
    if (_startTime < _endTime) {
      // during the day.
      newValue = ((ct >= _startTime) && (ct < _endTime));
    } else {
      // overnight.
      newValue = ((ct >= _startTime) || (ct < _endTime));
    }
  } // if

  if ((newValue == _value) && (_init)) {
    // no need to send an action.
  } else if (newValue) {
    _board->dispatch(_onAction);
    _board->dispatch(_valueAction, "1");

  } else {
    _board->dispatch(_offAction);
    _board->dispatch(_valueAction, "0");
  } // if
  _init = true;
  _value = newValue;
} // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void ScheduleElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
  callback("mode", _mode == Mode::TIMER ? "timer"
                   : _mode == Mode::ON  ? "on"
                                        : "off");
  callback(PROP_VALUE, _value ? "1" : "0");
} // pushState()

// End
