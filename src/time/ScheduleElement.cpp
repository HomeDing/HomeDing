/**
 * @file ScheduleElement.cpp
 * @brief Element Template class.
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


#include "time/ScheduleElement.h"
#include <ElementRegistry.h>
#include <Board.h>

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
  LOGGER_ETRACE("init()");
  Element::init(board);

  _startTime = TIME_T_NOT;
  _endTime = TIME_T_NOT;
} // init()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool ScheduleElement::set(const char *name, const char *value)
{
  LOGGER_ETRACE("set(%s, %s)", name, value);
  bool ret = true;

  if (_stricmp(name, "ontime") == 0) {
    _startTime = _atotime(value);

  } else if (_stricmp(name, "offtime") == 0) {
    _endTime = _atotime(value);

  } else if (_stricmp(name, "onon") == 0) {
    _onAction = value;

  } else if (_stricmp(name, "onoff") == 0) {
    _offAction = value;

  } else if (_stricmp(name, "onvalue") == 0) {
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
  LOGGER_ETRACE("start()");

  // Verify parameters
  _init = false;

  if ((_startTime == TIME_T_NOT) && (_endTime == TIME_T_NOT)) {
    LOGGER_EERR("no time span set.");
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
  bool val;

  if (ct > 0) {
    // There is a local time available.

    // find the current value
    if (_startTime < _endTime) {
      // during the day.
      val = ((ct >= _startTime) && (ct < _endTime));
    } else {
      // overnight.
      val = ((ct >= _startTime) || (ct < _endTime));
    }

    if ((val == _value) && (_init)) {
      // no need to send an action.
    } else if (val) {
      _board->dispatch(_onAction);
      _board->dispatch(_valueAction, "1");
    } else {
      _board->dispatch(_offAction);
      _board->dispatch(_valueAction, "0");
    }
    _value = val;
    _init = true;
  } // if
} // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void ScheduleElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
  callback("value", String(_value).c_str());
} // pushState()

/* ===== Register the Element ===== */

// As long as the Element is project specific or is a element always used
// the registration is placed here without using a register #define.

// When transferred to the HomeDing library a #define like the
// HOMEDING_INCLUDE_My should be used to allow the sketch to select the
// available Elements. See <HomeDing.h> the move these lines to
// time/ScheduleElement.h

// End
