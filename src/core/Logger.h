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
 * * 27.04.2019 add some delay(1) / yield(), enabling network events.
 */

#pragma once

/** send out errors to debug port and log file. */
#define LOGGER_LEVEL_ERR 0

/** Information to debug port and log file. Always. Requires loglevel 1(info) or more. */
#define LOGGER_LEVEL_INFO 1

/** detailed trace level to debug port only. Requires loglevel 2(trace). */
#define LOGGER_LEVEL_TRACE 2

// ===== RAW logging

#if defined(ESP8266) && defined(DEBUG_ESP_PORT)
#define LOGGER_RAW(...) \
  DEBUG_ESP_PORT.printf("  >" __VA_ARGS__); \
  DEBUG_ESP_PORT.println()

#elif defined(ESP32)
#define LOGGER_RAW(...) \
  log_printf("  >" __VA_ARGS__); \
  log_printf("\n");

#else
#define LOGGER_RAW(...)
#endif


#if defined(DEBUG_ESP_PORT) || defined(ESP32)

#define LOGGER_ENABLED

// ===== Logger Macros for non-element classes

#define LOGGER_ALWAYS(...) Logger::LoggerPrint(NULL, -1, __VA_ARGS__)

/** Send Information to Serial output only and always. */
#define LOGGER_JUSTINFO(...) Logger::LoggerPrint(NULL, LOGGER_LEVEL_INFO, __VA_ARGS__)

/** Send error to Logfile and serial output. */
#define LOGGER_ERR(...) Logger::LoggerPrint("sys", LOGGER_LEVEL_ERR, __VA_ARGS__)

/** Send information to logfile and serial output. */
#define LOGGER_INFO(...) Logger::LoggerPrint("sys", LOGGER_LEVEL_INFO, __VA_ARGS__)

/** Send trace information to serial output. */
#define LOGGER_TRACE(...) Logger::LoggerPrint(NULL, LOGGER_LEVEL_TRACE, __VA_ARGS__)

// ===== Logger Macros for Elements

#define LOGGER_EERR(...) Logger::LoggerEPrint(this, LOGGER_LEVEL_ERR, __VA_ARGS__)

#define LOGGER_EINFO(...) Logger::LoggerEPrint(this, LOGGER_LEVEL_INFO, __VA_ARGS__)

#define LOGGER_ETRACE(...) Logger::LoggerEPrint(this, LOGGER_LEVEL_TRACE, __VA_ARGS__)

#else

// no logger...
#define LOGGER_ALWAYS(...)
#define LOGGER_JUSTINFO(...)
#define LOGGER_ERR(...)
#define LOGGER_INFO(...)
#define LOGGER_TRACE(...)
#define LOGGER_EERR(...)
#define LOGGER_EINFO(...)
#define LOGGER_ETRACE(...)

#endif


#include <Board.h>    // Platform
#include <Element.h>  // Abstract Elements


class Logger {
public:
  static int logger_level;  // initialized to 0 === LOGGER_LEVEL_ERR;

  /**
   * @brief Create Logger entry line from given format and args.
   *
   * @param module module where the log line was created.
   * @param level level of the log line
   * @param fmt printf format
   * @param ... arguments
   */


  /*
   * initialize file system for log file
   * @param fs file system to be used.
   */
  static void init(FILESYSTEM *fs);

  /*
   * enable/disable log file
   * @param enable set true to enable file logging.
   */
  static void setLogFile(bool enable);

  static void LoggerPrint(const char *module, int level, const char *fmt, ...);
  static void LoggerEPrint(Element *module, int level, const char *fmt, ...);

private:
  // File System for logging file
  static FILESYSTEM *_fileSystem;

  // initialized to false === no logging to file;
  static bool _logFileEnabled;

  // Print log message
  static void _print(const char *module, int level, const char *fmt, va_list args);

  // Print to logfile
  static void _printToFile(char *buffer);
};


// end.
