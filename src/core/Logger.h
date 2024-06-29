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

/// LOGGER_ENABLED is defined when any Logging support is enabled at compile time.
/// It can be used to avoid computations before logging. 
#define LOGGER_ENABLED

// ===== Logger Macros for non-element classes

/// unconditional print this, no logging
#define LOGGER_PRINT(fmt, ...) printf(fmt "\n" __VA_OPT__(,) __VA_ARGS__)

/// unconditional print and log this
#define LOGGER_ALWAYS(...) Logger::LoggerPrint(NULL, -1, __VA_ARGS__)

/// Send error to print and logging when logger_level >= 0 (INFO).
#define LOGGER_ERR(...) Logger::LoggerPrint("sys", LOGGER_LEVEL_ERR, __VA_ARGS__)

/// Send error to print and logging when logger_level >= 1 (INFO).
#define LOGGER_INFO(...) Logger::LoggerPrint("sys", LOGGER_LEVEL_INFO, __VA_ARGS__)

/// Send error to print and logging when logger_level >= 2 (TRACE).
#define LOGGER_TRACE(...) Logger::LoggerPrint(NULL, LOGGER_LEVEL_TRACE, __VA_ARGS__)

/** deprecated: use LOGGER_PRINT */
#define LOGGER_JUSTINFO(...) LOGGER_PRINT(__VA_ARGS__)

// ===== Logger Macros for Elements

/// Send error to print and logging when logger_level of element or device >= 0 (INFO).
#define LOGGER_EERR(...) Logger::LoggerEPrint(this, LOGGER_LEVEL_ERR, __VA_ARGS__)

/// Send error to print and logging when logger_level of element or device >= 1 (INFO).
#define LOGGER_EINFO(...) Logger::LoggerEPrint(this, LOGGER_LEVEL_INFO, __VA_ARGS__)

/// Send error to print and logging when logger_level of element or device >= 2 (TRACE).
#define LOGGER_ETRACE(...) Logger::LoggerEPrint(this, LOGGER_LEVEL_TRACE, __VA_ARGS__)

#else

// no logger...
#define LOGGER_PRINT(fmt, ...) 
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

  /// @brief Create Raw Log entry without prefix
  /// @param fmt format string using printf syntax
  /// @param parameters according printf
  static void printf(const char *fmt, ...);

  static void flush();

  /// @brief Create Log entry with module prefix
  /// @param module module prefix as string
  /// @param level log level
  /// @param fmt format string using printf syntax
  /// @param parameters according printf
  static void LoggerPrint(const char *module, int level, const char *fmt, ...);

  /// @brief Create Log entry for Element
  /// @param module related Element
  /// @param level log level
  /// @param fmt format string using printf syntax
  /// @param parameters according printf
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
