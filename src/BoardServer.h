/**
 * @file boardServer.h
 * @brief Implementation of a web server request hander to handle the IoT board REST services.
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
 * Changelog:
 * * 30.05.2018 created by Matthias Hertel
 */

#ifndef BOARDSERVER_H
#define BOARDSERVER_H

#include <functional>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>


/**
 * @brief The BoardHandler is a local class of the main sketch that implements a
 * RequestHandler that is used to respond the state of the Elements on the
 * board.
 *
 * It is required not just to write a function but a minimal RequestHandler to
 * make the URL accessible. When called without a specifc Element id, all
 * Elements are reported back. When called with a specifc Element id only thie
 * Element is reported back.
 * @sample /$board
 * @sample /$board/button/f1
 */
class BoardHandler : public RequestHandler
{
public:
  /**
   * @brief Construct a new State Handler object
   * @param path The root path of the state ressources.
   * @param board reference to the board.
   */
  BoardHandler(const char *path, Board &board) : _path(path), _board(board)
  {
    DEBUG_MSG("BoardHandler:init: %s\n", _path.c_str());
  }

  /**
   * @brief Verify if the method and requestUri can be handles by this module.
   * @param requestMethod current http request method.
   * @param requestUri current url of the request.
   * @return true When the method and requestUri match a state request.
   */
  bool canHandle(HTTPMethod requestMethod, String requestUri) override
  {
    bool ret = ((requestMethod == HTTP_GET) || (requestMethod == HTTP_PUT)) &&
               (requestUri.startsWith(_path));
    // DEBUG_MSG("BoardHandler:canHandle(%s)=%u\n", requestUri.c_str(), ret);
    return (ret);
  }

  /**
   * @brief Handle the request of the state.
   * @param server reference to the server.
   * @param requestMethod current http request method.
   * @param requestUri current url of the request.
   * @return true When the state cound be retrieved.
   * @return false
   */
  bool handle(ESP8266WebServer &server, HTTPMethod requestMethod,
              String requestUri) override
  {
    DEBUG_MSG("BoardHandler:handle(%s)\n", requestUri.c_str());

    // cut off the registered path from the url.
    String localPath(requestUri.substring(_path.length()));

    if ((localPath.length() > 0) && (localPath.charAt(0) == '/')) {
      localPath.remove(0, 1);
    }

    int args = server.args();
    // DEBUG_MSG(" args=%d\n", args);

    if ((args == 0) && (requestMethod == HTTP_GET)) {
      String output;
      _board.getState(output, localPath);

      server.sendHeader("Cache-control", "NO-CACHE");
      server.send(200, "text/json", output);
      // DEBUG_LOG("  ret:%s\n", output.c_str());

    } else if ((args > 0) && (requestMethod == HTTP_GET)) {
      // like http://newdevice/$board/display/info?show=Hello
      // like http://logger/$board/display/f?show=27.30
      _board.setState(localPath, server.argName(0), server.arg(0));
      server.send(200, "text/plain", "");
    }

    return (true);
  } // handle()

protected:
  /**
   * @brief reference to the board.
   */
  Board _board;

  /**
   * @brief root path where the state ressource can be accessed. e.g. /$board.
   */
  String _path;
};

#endif
