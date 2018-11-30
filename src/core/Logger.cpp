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

#include <memory>
#include <sntp.h>
#include <time.h>

#include <FS.h>

#include <core/Logger.h>

#define LOGFILE_MAXSIZE (4 * 1024 - 200)

// String constants, only once in Memory
static const char *LOGGER_LEVELS = "eit";
static const char *LOGFILE_NAME = "/log.txt";
static const char *LOGFILE_OLD_NAME = "/log_old.txt";

void Logger::LoggerPrint(const char *module, int level, const char *fmt, ...)
{
  char buffer[200];

  if (level <= logger_level) {
    _printPrefix(buffer, module, level);
    char *p = strchr(buffer, '\0');

    va_list args;
    va_start(args, fmt);
    vsnprintf(p, sizeof(buffer) - 40, fmt, args);
    va_end(args);
#ifdef DEBUG_ESP_PORT
    DEBUG_ESP_PORT.println(buffer);
#endif
    if (level < LOGGER_LEVEL_TRACE) {
      _printToFile(buffer);
    }

  } // if
} // LoggerPrint


void Logger::LoggerEPrint(Element *elem, int level, const char *fmt, ...)
{
  char buffer[200];

  if ((level <= logger_level) || (level <= elem->loglevel)) {
    _printPrefix(buffer, elem->id, level);
    char *p = strchr(buffer, '\0');

    va_list args;
    va_start(args, fmt);
    vsnprintf(p, sizeof(buffer) - 40, fmt, args);
    va_end(args);
#ifdef DEBUG_ESP_PORT
    DEBUG_ESP_PORT.println(buffer);
#endif
    if (level < LOGGER_LEVEL_TRACE) {
      _printToFile(buffer);
    }

  } // if
} // LoggerEPrint


void Logger::_printPrefix(char *buffer, const char *module, int level)
{
  uint32 current_stamp = sntp_get_current_timestamp();
  char timeBuffer[16];

  // LOGGER_RAW("getTime=%d", current_stamp);

  if (current_stamp < (30 * 24 * 60 * 60)) {
    // using millis() when no time is available
    current_stamp = millis()/1000;
  } // if

  struct tm *tmp = localtime((const time_t *)(&current_stamp));
  strftime(timeBuffer, sizeof(timeBuffer), "%H:%M:%S", tmp);
  if (module) {
  sprintf(buffer, "%s %s:%c:", timeBuffer, module, *(LOGGER_LEVELS + level));
  } else {
    sprintf(buffer, "%s ", timeBuffer);
  }
};


void Logger::_printToFile(char *buffer)
{
  File f = SPIFFS.open(LOGFILE_NAME, "a");

  if (f.size() > LOGFILE_MAXSIZE) {
    // rename to LOGFILE_OLD_NAME
    f.close();
    SPIFFS.remove(LOGFILE_OLD_NAME);
    SPIFFS.rename(LOGFILE_NAME, LOGFILE_OLD_NAME);
    f = SPIFFS.open(LOGFILE_NAME, "a");
  } // if
  f.println(buffer);
  f.close();
};

// Default: Log INFO and ERROR
int Logger::logger_level = LOGGER_LEVEL_ERR;
// end.
