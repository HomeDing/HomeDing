/**
 * @file AlarmElement.cpp
 * @brief Create Action based on a specified time of the day.
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
 * Changelog:see AlarmElement.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <time/AlarmElement.h>

/* ===== Define local constants and often used strings ===== */

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new AlarmElement
 * @return AlarmElement* created element
 */
Element *AlarmElement::create()
{
  return (new AlarmElement());
} // create()


/* ===== Element functions ===== */

AlarmElement::AlarmElement() {
  startupMode = Element_StartupMode::Time;
}


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool AlarmElement::set(const char *name, const char *value)
{
  bool ret = true;

  if (_stricmp(name, "time") == 0) {
    _time = _atotime(value);

  } else if (_stricmp(name, "onTime") == 0) {
    _timeAction = value;

  } else {
    ret = Element::set(name, value);
  } // if

  return (ret);
} // set()


/**
 * @brief Activate the AlarmElement.
 */
void AlarmElement::start()
{
  // TRACE("start()");

  // Verify parameters

  // if (parameters ok) {
  Element::start();
  // } // if

} // start()


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void AlarmElement::loop()
{
  time_t ct = _board->getTimeOfDay();

  if (ct > 0) {
    if ((_lastTime < _time) && (ct >= _time)) {
      // send alarm
      _board->dispatch(_timeAction);
    }
    _lastTime = ct;
  } // if
} // loop()


// End
