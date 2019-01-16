/**
 * @file boardServer.cpp
 * @brief Implementation of a web server request hander to handle the IoT board
 * REST services.
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
 * Changelog: see BoardServer.h
 */

#include <BoardServer.h>

#define SVC_ROOT "/$board"

/**
 * @brief Construct a new State Handler object
 * @param path The root path of the state ressources.
 * @param board reference to the board.
 */
BoardHandler::BoardHandler(Board *board)
{
  // LOGGER_RAW("BoardHandler:init: %s", path);
  _path = SVC_ROOT;
  _board = board;
}

/**
 * @brief Verify if the method and requestUri can be handles by this module.
 * @param requestMethod current http request method.
 * @param requestUri current url of the request.
 * @return true When the method and requestUri match a state request.
 */
bool BoardHandler::canHandle(HTTPMethod requestMethod, String requestUri)
{
  bool ret = ((requestMethod == HTTP_GET) && (requestUri.startsWith(_path)));
  // LOGGER_RAW("BoardHandler:canHandle(%s)=%u", requestUri.c_str(), ret);
  return (ret);
}

/**
 * @brief Handle the request of the state.
 * @param server reference to the server.
 * @param requestMethod current http request method.
 * @param requestUri current url of the request.
 * @return true When the state could be retrieved.
 * @return false
 */
bool BoardHandler::handle(ESP8266WebServer &server, HTTPMethod requestMethod,
            String requestUri)
{
  // LOGGER_RAW("BoardHandler:handle(%s)", requestUri.c_str());

  // cut off the registered path from the url.
  String localPath(requestUri.substring(_path.length()));

  if ((localPath.length() > 0) && (localPath.charAt(0) == '/')) {
    localPath.remove(0, 1);
  }

  int args = server.args();
  // LOGGER_RAW(" args=%d", args);

  if (args > 0) {
    // send action to the specified element
    _board->setState(localPath, server.argName(0), server.arg(0));
    server.send(200, "text/plain", "");

  } else if (args == 0) {
    // get status of all or the specified element
    String output;
    _board->getState(output, localPath);

    server.sendHeader("Cache-control", "NO-CACHE");
    server.send(200, "text/json", output);
    // DEBUG_LOG("  ret:%s\n", output.c_str());
  } // if

  return (true);
} // handle()

// End.