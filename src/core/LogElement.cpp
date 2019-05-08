/**
 * @file LogElement.cpp
 * @brief The Log Element implements saving sensor data to the local filöe system.
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
 * Changelog:see LogElement.h
 */

#include <Arduino.h>
#include <Element.h>
#include <Board.h>

#include "LogElement.h"
#include <ElementRegistry.h>

#include <FS.h>

/* ===== Define local constants and often used strings ===== */

#define LOGFILE_MAXSIZE (4 * 1024 - 200)

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new LogElement
 * @return LogElement* created element
 */
Element *LogElement::create()
{
  return (new LogElement());
} // create()


/* ===== Element functions ===== */

LogElement::LogElement()
{
  startupMode = Element_StartupMode::Time;
}


void LogElement::_logToFile()
{
  LOGGER_ETRACE("log(%d,%s)", _timestamp, _value.c_str());

  File f = SPIFFS.open(_logfileName, "a");

  if (f.size() > LOGFILE_MAXSIZE) {
    // rename to LOGFILE_OLD_NAME
    f.close();
    SPIFFS.remove(_logfileOldName);
    SPIFFS.rename(_logfileName, _logfileOldName);
    f = SPIFFS.open(_logfileName, "a");
  } // if
  f.print(_timestamp);
  f.print(',');
  f.println(_value);
  f.close();
};


void LogElement::init(Board *board)
{
  LOGGER_ETRACE("init()");
  Element::init(board);
  // do something here like initialization
  _changed = false;
} // init()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool LogElement::set(const char *name, const char *value)
{
  LOGGER_ETRACE("set(%s, %s)", name, value);
  bool ret = true;

  if (_stricmp(name, "filename") == 0) {
    // name of logfile
    _logfileName = value;
    _logfileOldName = value;
    _logfileOldName.replace(".", "_old.");

    // } else if (_stricmp(name, "doAction") == 0) {
    // make something

  } else if (_stricmp(name, "value") == 0) {
    if (active) {
      _value = value;
      _timestamp = _board->getTime();
      _changed = true;
    }

  } else {
    ret = Element::set(name, value);
  } // if

  return (ret);
} // set()


/**
 * @brief Activate the LogElement.
 */
void LogElement::start()
{
  LOGGER_ETRACE("start(%s)", _logfileName.c_str());

  // Verify parameters
  if (_logfileName.length() > 0) {
    Element::start();
  } // if
} // start()


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void LogElement::loop()
{
  // do something
  if (_changed)
    _logToFile();
  _changed = false;
} // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void LogElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
  callback("value", String(_value).c_str());
} // pushState()


// End
