/**
 * @file boardServer.h
 * @brief Implementation of a web server request hander to handle the IoT board
 * REST services to get the current state of elements and send actions to active
elements.
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
 *
 * @details
@verbatim
  This handler registers to all http GET request to the base url `/$board`.
  

  The state of all existing elements can be retrieved by using the base url
like: <http://devding/$board>.

  The state of a special existing element can be retrieved by using a more
specific url like: <http://devding/$board/timer/blink>.

  To send an action to a element a parameter can be added like:
  <http://devding/$board/displaydot/b?value=1> or
<http://devding/$board/displaytext/info?show=Hello>

@endverbatim
 */

#ifndef BOARDSERVER_H
#define BOARDSERVER_H

#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>


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
   * @brief Construct a new BoardHandler object
   * @param path The root path of the state ressources.
   * @param board reference to the board.
   */
  BoardHandler(Board *board);

  /**
   * @brief Verify if the method and requestUri can be handles by this module.
   * @param requestMethod current http request method.
   * @param requestUri current url of the request.
   * @return true When the method and requestUri match a state request.
   */
  bool canHandle(HTTPMethod requestMethod, String requestUri);

  /**
   * @brief Handle the request of the state.
   * @param server reference to the server.
   * @param requestMethod current http request method.
   * @param requestUri current url of the request.
   * @return true When the state cound be retrieved.
   * @return false
   */
  bool handle(ESP8266WebServer &server, HTTPMethod requestMethod,
              String requestUri);


protected:
  /**
   * @brief reference to the board.
   */
  Board *_board;

  /**
   * @brief root path where the state ressource can be accessed. e.g. /$board.
   */
  String _path;

  void handleReboot(ESP8266WebServer &server, bool wipe = false);
  void handleElements(ESP8266WebServer &server);

};

#endif
