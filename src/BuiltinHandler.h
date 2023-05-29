/**
 * @file BuiltinHandler.h
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
 * Changelog:
 * * 25.11.2021 created by Matthias Hertel
 *
 * @details
@verbatim
This handler registers to http GET request for
  * /$setup - configure wifi network name and passphrase
  * /$update - update Web files from github repository
  * /$upload - manual upload files
@endverbatim
 */

#pragma once

#include <MicroJsonComposer.h>

/**
 * @brief The BuiltinHandler is a class that implements the RequestHandler interface.
 */
class BuiltinHandler : public RequestHandler {
public:
  /**
   * @brief Construct a new BuiltinHandler object
   * @param board reference to the board.
   */
  BuiltinHandler(Board *board);

  /**
   * @brief Verify if the method and requestUri can be handles by this module.
   * @param requestMethod current http request method.
   * @param requestUri current url of the request.
   * @return true When the method and requestUri match a state request.
   */
#if defined(ESP8266)
  bool canHandle(HTTPMethod requestMethod, const String &requestUri) override;
#elif defined(ESP32)
  bool canHandle(HTTPMethod requestMethod, String requestUri) override;
#endif

  /**
   * @brief Handle the request of the state.
   * @param server reference to the server.
   * @param requestMethod current http request method.
   * @param requestUri current url of the request.
   * @return true When the state could be retrieved.
   * @return false
   */
#if defined(ESP8266)
  bool handle(WebServer &server, HTTPMethod requestMethod, const String &requestUri) override;
#elif defined(ESP32)
  bool handle(WebServer &server, HTTPMethod requestMethod, String requestUri) override;
#endif


private:
  /**
   * @brief reference to the board.
   */
  Board *_board;
};
