/**
 * @file boardServer.h
 * @brief Implementation of a web server request hander to handle the IoT board
 * REST services to get the current state of elements and send actions to active
 * elements.
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
 * * 21.12.2018 refactoring.
 * * 31.07.2021 include handling redirect on "/" request.
 * * 23.04.2023 using the $board for calling services is removed
 *   $xxx will only be used for files included in the firmware. 
 * @details

@verbatim
This handler registers to all http GET request to the urls starting with `/api`.

The state of all existing elements can be retrieved by using the base url
like: <http://homeding/api/state>.

The state of a special existing element can be retrieved by using a more
specific url like: <http://homeding/api/state/timer/blink>.

To send an action to a element a parameter can be added like:
<http://homeding/api/state/value/x?value=11>
<http://homeding/api/state/displaytext/info?show=Hello>
@endverbatim
 */

#pragma once
#include <MicroJsonComposer.h>

/**
 * @brief The BoardHandler is a local class of the main sketch that implements a
 * RequestHandler that is used to respond the state of the Elements on the
 * board.
 *
 * It is required not just to write a function but a minimal RequestHandler to
 * make the URL accessible. When called without a specifc Element id, all
 * Elements are reported back. When called with a specifc Element id only thie
 * Element is reported back.
 * @sample /api/state
 * @sample /api/state/button/f1
 */
class BoardHandler : public RequestHandler {
public:
  /**
   * @brief Construct a new BoardHandler object
   * @param board reference to the board.
   */
  BoardHandler(Board *board);

  /**
   * @brief Verify if the method and requestUri can be handles by this module.
   * @param requestMethod current http request method.
   * @param requestUri current url of the request.
   * @return true When the method and requestUri match a state request.
   */
#if defined(ESP8266)
  bool canHandle(HTTPMethod requestMethod, const String &requestUri) override;
#elif defined(ESP32)

#if (ESP_ARDUINO_VERSION_MAJOR < 3)
  bool canHandle(HTTPMethod requestMethod, String uri) override;
#else
  bool canHandle(WebServer &server, HTTPMethod requestMethod, const String &uri) override;
#endif

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
  bool handle(WebServer &server, HTTPMethod /* requestMethod */, const String &requestUri) override;
#elif defined(ESP32)
  bool handle(WebServer &server, HTTPMethod requestMethod, const String &requestUri) override;
#endif


protected:
  /**
   * @brief reference to the board.
   */
  Board *_board;

  /**
   * @brief execute a restart or reboot 
   * @param wipe if true unregister from the network.
  */
  void handleReboot(WebServer &server, bool wipe = false);

  /** @brief Return list of local networks. */
  String handleScan();

  /**
   * @brief Use url parameters to establish / verify a WiFi connection.
   * @param server The Web Server on the access point.
   */
  void handleConnect(WebServer &server);

private:
  // list files in filesystem recursively.
  void handleListFiles(MicroJsonComposer &jc, String path);

  // clean out all files in filesystem except config and secret files.
  void handleCleanWeb(String path);

};
