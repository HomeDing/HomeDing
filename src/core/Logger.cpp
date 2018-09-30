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

#include "core/Logger.h"

void Logger::LoggerPrint(const char *module, int level, const char *fmt, ...)
{
  char buffer[200];

  if (level <= logger_level) {
    // Logger prefix "><module>:<loglevel>: ....."
    const char *levString;
    if (level == LOGGER_LEVEL_ERR) {
      levString = "e";
    } else if (level == LOGGER_LEVEL_LOG) {
      levString = "l";
    } else if (level == LOGGER_LEVEL_TRACE) {
      levString = "t";
    } else if (level == LOGGER_LEVEL_INFO) {
      levString = "i";
    } else {
      levString = "---";
    }
    snprintf(buffer, sizeof(buffer), ">%s:%s: ", module, levString);
#ifdef DEBUG_ESP_PORT
    DEBUG_ESP_PORT.print(buffer);
#endif

    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
#ifdef DEBUG_ESP_PORT
    DEBUG_ESP_PORT.println(buffer);
#endif
  } // if
} // LoggerPrint

int Logger::logger_level = LOGGER_LEVEL_LOG; //  LOGGER_LEVEL_ERR;


// end.
