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
#include <HomeDing.h>

#include <core/LogElement.h>

#include <FS.h>

#define TRACE(...) LOGGER_ETRACE(__VA_ARGS__)

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
  TRACE("log(%d,%s)", _timestamp, _value.c_str());

  File f = _board->fileSystem->open(_logfileName, "a");

  if (f.size() > _filesize) {
    // rename to LOGFILE_OLD_NAME
    f.close();
    _board->fileSystem->remove(_logfileOldName);
    _board->fileSystem->rename(_logfileName, _logfileOldName);
    f = _board->fileSystem->open(_logfileName, "a");
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
  TRACE("set %s = %s", name, value);
  bool ret = true;

  if (_stricmp(name, PROP_VALUE) == 0) {
    if (active) {
      loop(); // be sure the current average time-span is saved.

      if (_avgTime > 0) {
        if (!_avgEnd) {
          TRACE("start avg");

          unsigned long now = millis();
          // start a new build average values
          _timestamp = time(nullptr) + _avgTime / 2 / 1000; // timestamp to record the average
          _avgEnd = now + _avgTime;
TRACE("_avgEnd=%d", _avgEnd);
          _avgCount = 0;
          _avgSum = 0;
        }
          TRACE("add avg");
        _avgCount++;
        _avgSum += atof(value);

      } else {
          TRACE("direct value");
        _value = value;
        _timestamp = time(nullptr);
        _changed = true;
      }
    }

  } else if (_stricmp(name, "averagetime") == 0) {
    _avgTime = _atotime(value) * 1000;
TRACE("_avgTime=%d", _avgTime);

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
  unsigned long now = millis();

  // check for average calculation end
  if ((_avgTime) && (_avgEnd) && (now > _avgEnd)) {
TRACE("now=%d", now);
    _value = String(_avgSum / _avgCount, 2);
    TRACE("calc(%s)", _value.c_str());
    _changed = true;
    _avgEnd = 0;
  }

  // save to file
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
