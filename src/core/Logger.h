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
 * 04.06.2018 creation
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include <memory>

#define LOGGER_LEVEL_INFO -1
#define LOGGER_LEVEL_ERR 0
#define LOGGER_LEVEL_LOG 1
#define LOGGER_LEVEL_TRACE 2

#ifndef LOGGER_MODULE
#define LOGGER_MODULE "sketch"
#endif

#define LOGGER_INFO(...)                                                       \
  Logger::LoggerPrint(LOGGER_MODULE, LOGGER_LEVEL_INFO, __VA_ARGS__)

#define LOGGER_ERR(...)                                                        \
  Logger::LoggerPrint(LOGGER_MODULE, LOGGER_LEVEL_ERR, __VA_ARGS__)

#define LOGGER_LOG(...)                                                        \
  Logger::LoggerPrint(LOGGER_MODULE, LOGGER_LEVEL_LOG, __VA_ARGS__)

#define LOGGER_TRACE(...)                                                      \
  Logger::LoggerPrint(LOGGER_MODULE, LOGGER_LEVEL_TRACE, __VA_ARGS__)

class Logger
{
public:
  static int logger_level; // initialized to 0 === LOGGER_LEVEL_ERR;

  /**
   * @brief Create Logger entry line from given format and args.
   *
   * @param module module where the log line was created.
   * @param level level of the log line
   * @param fmt printf format
   * @param ... arguments
   */
  static void LoggerPrint(const char *module, int level, const char *fmt, ...);
};

#endif

// end.
