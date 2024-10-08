/**
 * @file Logger.cpp
 * @brief Implementation of a logging utility that logs to the Serial port and
 * to a buffer to support for http access to error logs.
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license,
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino.
 *
 * Changelog:
 * see Logger.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <stdio.h>
#include <time.h>

#include <core/Logger.h>
#include "Logger.h"

#define LOGFILE_MAXSIZE (4 * 1024 - 200)

// String constants, only once in Memory
static const char *LOGGER_LEVELS = "eit";  // error, info, trace

#if !defined(HD_MINIMAL)
static const char *LOGFILE_NAME = "/log.txt";
static const char *LOGFILE_OLD_NAME = "/log_old.txt";
#endif

// initialize file system for log file
void Logger::init(FILESYSTEM *fs) {
  _fileSystem = fs;
}  // init()


// enable/disable log file
void Logger::setLogFile(bool enable) {
  _logFileEnabled = enable;
}  // setLogFile()


void Logger::flush() {
#ifdef DEBUG_ESP_PORT
  DEBUG_ESP_PORT.flush();

#elif ARDUINO_USB_CDC_ON_BOOT  // Serial used for USB CDC
  if (Serial.isConnected())
    Serial.flush();

#elif defined(ESP32)
  Serial.flush();

#endif
}

/**
 * @brief Print out logging information
 */
void Logger::_print(const char *module, int level, const char *fmt,
                    va_list args) {
  char buffer[200];
  char *p = buffer;  // end of string : next segment of logline

  // timestamp into buffer, using millis() when no time is available
  time_t now = time(nullptr);
  if (!now)
    now = Board::getSeconds();
  p += strftime(p, sizeof(buffer), "%H:%M:%S ", localtime(&now));  // %T

  if (module) {
    // add module and loglevel
    p += sprintf(p, "%s:%c\t", module, *(LOGGER_LEVELS + level));
  } else {
    p += sprintf(p, ">");
  }

  // message into buffer
  vsnprintf(p, sizeof(buffer) - 40, fmt, args);

#ifdef DEBUG_ESP_PORT
  DEBUG_ESP_PORT.println(buffer);
#elif ARDUINO_USB_CDC_ON_BOOT  // Serial used for USB CDC
  if (Serial.isConnected())
    Serial.println(buffer);

#elif defined(ESP32)
  Serial.println(buffer);
#endif
  hd_yield();

  if ((module) && (_logFileEnabled) && (level < LOGGER_LEVEL_TRACE)) {
    _printToFile(buffer);
    hd_yield();
  }  // if
}  // _print


void Logger::_printToFile(char *buffer) {
#if !defined(HD_MINIMAL)
  if (_fileSystem) {
    File f = _fileSystem->open(LOGFILE_NAME, "a");

    if (f.size() > LOGFILE_MAXSIZE) {
      // rename to LOGFILE_OLD_NAME
      f.close();
      _fileSystem->remove(LOGFILE_OLD_NAME);
      _fileSystem->rename(LOGFILE_NAME, LOGFILE_OLD_NAME);
      hd_yield();
      f = _fileSystem->open(LOGFILE_NAME, "a");
    }  // if
    f.println(buffer);
    f.close();
    hd_yield();
  }
#endif
};


/// @brief Create Raw Log entry without prefix
/// @param fmt format string using printf syntax
/// @param parameters according printf
void Logger::printf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  Logger::_print(nullptr, LOGGER_LEVEL_INFO, fmt, args);
  va_end(args);
}  // printf


/**
 * @brief Print Log entry from system (not element)
 */
void Logger::LoggerPrint(const char *module, int level, const char *fmt, ...) {
  if (level <= logger_level) {
    va_list args;
    va_start(args, fmt);
    Logger::_print(module, level, fmt, args);
    va_end(args);
  }  // if
  hd_yield();
}  // LoggerPrint


/**
 * @brief Print Log entry from element.
 */
void Logger::LoggerEPrint(Element *elem, int level, const char *fmt, ...) {
  if ((level <= logger_level) || (level <= elem->loglevel)) {
    va_list args;
    va_start(args, fmt);
    Logger::_print(elem->id, level, fmt, args);
    va_end(args);
  }  // if
  hd_yield();
}  // LoggerEPrint


// Default: Log INFO and ERROR
int Logger::logger_level = LOGGER_LEVEL_INFO;

bool Logger::_logFileEnabled = false;

FILESYSTEM *Logger::_fileSystem = nullptr;


// end.
