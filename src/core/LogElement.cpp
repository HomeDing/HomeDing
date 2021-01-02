/**
 * @file LogElement.cpp
 * @brief The Log Element implements saving sensor data to the local file system.
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
#include <Board.h>
#include <Element.h>

#include "LogElement.h"

#include <FS.h>

/* ===== Define local constants and often used strings ===== */

#define LOGFILE_DEFAULTSIZE (4 * 1024 - 200)

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
  _filesize = LOGFILE_DEFAULTSIZE;
}


void LogElement::_logToFile()
{
  // TRACE("log(%d,%s)", _timestamp, _value.c_str());

  File f = SPIFFS.open(_logfileName, "a");

  if (f.size() > _filesize) {
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
  Element::init(board);
  // do something here like initialization
  _changed = false;
} // init()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool LogElement::set(const char *name, const char *value)
{
  bool ret = true;

  if (_stricmp(name, PROP_VALUE) == 0) {
    if (active) {
      _value = value;
      _timestamp = time(nullptr);
      _changed = true;
    }

  } else if (_stricmp(name, "filesize") == 0) {
    _filesize = _atoi(value);

  } else if (_stricmp(name, "filename") == 0) {
    // name of logfile
    _logfileName = value;
    _logfileOldName = value;
    _logfileOldName.replace(".", "_old.");

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
  // TRACE("start(%s)", _logfileName.c_str());

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
  callback(PROP_VALUE, String(_value).c_str());
} // pushState()


// End
