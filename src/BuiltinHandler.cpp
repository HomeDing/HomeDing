/**
 * @file BuiltinHandler.cpp
 * @brief WebServer for some minimal functionality to configure network and upload initial web server files.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license,
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino.
 *
 * Changelog: see BuiltinHandler.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <BuiltinHandler.h>

#include "upload.h"

// use TRACE for compiling with detailed TRACE output.
#define TRACE(...) // LOGGER_JUSTINFO(__VA_ARGS__)

/**
 * @brief Construct a new BuiltinHandler object
 * @param board reference to the board.
 */
BuiltinHandler::BuiltinHandler(Board *board) {
  // TRACE("BuiltinHandler:init: %s", path);
  _board = board;
}

// ===== board specific services

/**
 * @brief Verify if the method and requestUri can be handles by this module.
 * @param requestMethod current http request method.
 * @param requestUri current url of the request.
 * @return true When the method and requestUri match a state request.
 */
#if defined(ESP8266)
bool BuiltinHandler::canHandle(HTTPMethod requestMethod, const String &requestUri)
#elif defined(ESP32)
bool BuiltinHandler::canHandle(HTTPMethod requestMethod, String requestUri)
#endif
{
  bool can = ((!_board->isSafeMode) && (requestMethod == HTTP_GET) // only GET requests in the API
              && (requestUri.startsWith("/$setup") || requestUri.startsWith("/$update") || requestUri.startsWith("/$upload")));
  return (can);
} // canHandle


/**
 * @brief Handle the request of the state.
 * @param server reference to the server.
 * @param requestMethod current http request method.
 * @param requestUri current url of the request.
 * @return true When the state could be retrieved.
 * @return false
 */
#if defined(ESP8266)
bool BuiltinHandler::handle(WebServer &server, UNUSED HTTPMethod requestMethod, const String &uri)
#elif defined(ESP32)
bool BuiltinHandler::handle(WebServer &server, HTTPMethod requestMethod, String uri)
#endif
{
  TRACE("handle(%s)", uri.c_str());
  String output;

  if (_board->isSafeMode) {
    return(false);

  } else if (uri.startsWith("/$setup")) {
    // Network Config Page
    output = FPSTR(setupContent);

  } else if (uri.startsWith("/$update")) {
    // Bootstrap Page
    output = FPSTR(updateContent);

#if ! defined(HD_MINIMAL)
  } else if (uri.startsWith("/$upload")) {
    // Bulk File Upload Page
    output = FPSTR(uploadContent);
#endif

  } else {
    return(false);
  }

  server.sendHeader("X-Content-Type-Options", "no-sniff");
  server.send(200, "text/html", output);

  return (true);
} // handle()

// End.