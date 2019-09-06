/**
 * @file TimeElement.cpp
 *
 * @brief Core Element for the HomeDing Library to send actions based on the actual local time.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license,
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino.
 *
 * Changelog: see TimeElement.h
 */

#include <Arduino.h>
#include <Element.h>
#include <Board.h>

#include <ElementRegistry.h>
#include <time/TimeElement.h>

#include "time.h"

static const char *TIME_ontime = "ontime";
static const char *TIME_onminute = "onminute";
static const char *TIME_ondate = "ondate";
static const char *TIME_ontimestamp = "ontimestamp";

static const char *TIME_timeFmt = "%H:%M:%S";
static const char *TIME_minuteFmt = "%H:%M";
static const char *TIME_dateFmt = "%Y-%m-%d";
static const char *TIME_timestampFmt = "%Y-%m-%d %H:%M:%S";

/**
 * @brief static factory function to create a new TimeElement
 * @return TimeElement* created element
 */
Element *TimeElement::create()
{
  return (new TimeElement());
} // create()


/* ===== Element functions ===== */

TimeElement::TimeElement() {
  startupMode = Element_StartupMode::Time;
}


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool TimeElement::set(const char *name, const char *value)
{
  bool ret = true;

  if (_stricmp(name, TIME_ontime) == 0) {
    _timeAction = value;

  } else if (_stricmp(name, TIME_onminute) == 0) {
    _minuteAction = value;

  } else if (_stricmp(name, TIME_ondate) == 0) {
    _dateAction = value;

  } else if (_stricmp(name, TIME_ontimestamp) == 0) {
    _timestampAction = value;

  } else {
    ret = Element::set(name, value);
  } // if
  return (ret);
} // set()


/**
 * @brief Activate the TimeElement.
 */
void TimeElement::start()
{
  LOGGER_ETRACE("start()");

  // set some defaults
  _lastTimestamp = 0;
  _lastMinute = 0;
  _lastDate = 0;

  Element::start();
} // start()


/**
 * @brief check the state of the DHT values and eventually create actions.
 */
void TimeElement::loop()
{
  time_t ct = _board->getTime();

  if (ct > 0) {
    // check for time actions...
    if (_lastTimestamp != ct) {
      _sendAction(_timeAction, TIME_timeFmt, ct);
      _sendAction(_timestampAction, TIME_timestampFmt, ct);
      _lastTimestamp = ct;
    } // if

    // ignore seconds
    ct -= (ct % 60);
    if (_lastMinute != ct) {
      _sendAction(_minuteAction, TIME_minuteFmt, ct);
      _lastMinute = ct;
    } // if

    // ignore time
    ct -= (ct % (24 * 60 * 60));
    if (_lastDate != ct) {
      _sendAction(_dateAction, TIME_dateFmt, ct);
      _lastDate = ct;
    } // if
  } // if
} // loop()


// ===== private functions =====

/**
 * @brief Send out the action after adding the formatted value.
 * @param action Action template.
 * @param fmt Format for the value.
 * @param tStamp the time
 */
void TimeElement::_sendAction(String &action, const char *fmt, time_t tStamp)
{
  if (action.length()) {
    char b[32];
    struct tm *tmp = localtime(&tStamp);
    strftime(b, sizeof(b), fmt, tmp);
    _board->dispatch(action, b);
  } // if
} // _sendAction()


// ===== Register =====

// Register the TimeElement onto the ElementRegistry.
bool TimeElement::registered =
    ElementRegistry::registerElement("time", TimeElement::create);

// End.
