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

#include <FS.h>

#include <core/Logger.h>

void Logger::LoggerPrint(const char *module, int level, const char *fmt, ...)
{
  char buffer[200];

  if (level <= logger_level) {
    printPrefix(buffer, module, level);
    char *p = strchr(buffer, '\0');

    va_list args;
    va_start(args, fmt);
    vsnprintf(p, sizeof(buffer) - 40, fmt, args);
    va_end(args);
#ifdef DEBUG_ESP_PORT
    DEBUG_ESP_PORT.println(buffer);
#endif
    if (level < LOGGER_LEVEL_TRACE) {
      printFile(buffer);
    }

  } // if
} // LoggerPrint


void Logger::LoggerEPrint(Element *elem, int level, const char *fmt, ...)
{
  char buffer[200];

  if ((level <= logger_level) || (level <= elem->loglevel)) {
    printPrefix(buffer, elem->id, level);
    char *p = strchr(buffer, '\0');

    va_list args;
    va_start(args, fmt);
    vsnprintf(p, sizeof(buffer) - 40, fmt, args);
    va_end(args);
#ifdef DEBUG_ESP_PORT
    DEBUG_ESP_PORT.println(buffer);
#endif
    if (level < LOGGER_LEVEL_TRACE) {
      printFile(buffer);
    }

  } // if
} // LoggerEPrint


void Logger::printPrefix(char *buffer, const char *module, int level)
{
  const char *levString = "eit";
  sprintf(buffer, ">%s:%c:", module, *(levString + level));
};

void Logger::printFile(char *buffer)
{
  File f = SPIFFS.open("/log.txt", "a");
  int s = f.size();
  LOGGER_RAW("log.txt size=%d", s);
  if (s < (4 * 1024 - 100)) {
    f.println(buffer);
  }
  f.close();
};

int Logger::logger_level = LOGGER_LEVEL_ERR; // LOGGER_LEVEL_ERR;


// end.
