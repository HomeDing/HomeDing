/**
 * @file Logger.h
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
 * * 04.06.2018 creation
 * * 24.10.2018 logging for elements using element->id
 * *   logging for elements using element->loglevel;
 * *   log errors and info to file log.txt
 * * 27.10.2018 rolling logfiles and log_old.txt.
 * * 02.02.2019 reduce Flash memory, optimizing
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include <cstdarg> 

// information to debug port and log file.
#define LOGGER_LEVEL_INFO 0

// always send out errors to debug port and log file.
#define LOGGER_LEVEL_ERR 1

// detailed trace level to debug port only.
#define LOGGER_LEVEL_TRACE 2

// for non-element classes

#define LOGGER_JUSTINFO(...)                                                   \
  Logger::LoggerPrint(NULL, LOGGER_LEVEL_INFO, __VA_ARGS__)

#define LOGGER_INFO(...)                                                       \
  Logger::LoggerPrint("sys", LOGGER_LEVEL_INFO, __VA_ARGS__)

#define LOGGER_ERR(...)                                                        \
  Logger::LoggerPrint("sys", LOGGER_LEVEL_ERR, __VA_ARGS__)

// send some logging to serial output
#define LOGGER_TRACE(...)                                                      \
  Logger::LoggerPrint("sys", LOGGER_LEVEL_TRACE, __VA_ARGS__)

#ifdef DEBUG_ESP_PORT
#define LOGGER_RAW(...)                                                        \
  DEBUG_ESP_PORT.printf("  >" __VA_ARGS__);                                    \
  DEBUG_ESP_PORT.print("\n")
#else
#define LOGGER_RAW(...)
#endif

// for Elements:

#define LOGGER_EERR(...)                                                       \
  Logger::LoggerEPrint(this, LOGGER_LEVEL_ERR, __VA_ARGS__)

#define LOGGER_EINFO(...)                                                      \
  Logger::LoggerEPrint(this, LOGGER_LEVEL_INFO, __VA_ARGS__)

#define LOGGER_ETRACE(...)                                                     \
  Logger::LoggerEPrint(this, LOGGER_LEVEL_TRACE, __VA_ARGS__)


#include <Element.h>

class Logger
{
public:
  static int logger_level; // initialized to 0 === LOGGER_LEVEL_ERR;

  static bool logger_file; // initialized to false === no logging to file;

  /**
   * @brief Create Logger entry line from given format and args.
   *
   * @param module module where the log line was created.
   * @param level level of the log line
   * @param fmt printf format
   * @param ... arguments
   */

  static void LoggerPrint(const char *module, int level, const char *fmt, ...);
  static void LoggerEPrint(Element *module, int level, const char *fmt, ...);

private:
  // Print log message
  static void _print(const char *module, int level, const char *fmt, va_list args);

  // Print to logfile
  static void _printToFile(char *buffer);
};

#endif

// end.
