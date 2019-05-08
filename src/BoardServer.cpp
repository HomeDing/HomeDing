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

#include <Arduino.h>
#include <Element.h>
#include <Board.h>

#include <BoardServer.h>
#include <ElementRegistry.h>

#include "upload.h"
#include <FS.h>

#define SVC_ANY "/$"

#define SVC_REBOOT "/$reboot"
#define SVC_RESET "/$reset"
#define SVC_SYSINFO "/$sysinfo"

#define SVC_ELEMENTS "/$elements"
#define SVC_UPLOAD "/$upload"
#define SVC_LISTFILES "/$list"

#define SVC_BOARD "/$board"

static const char *TEXT_JSON = "text/json";
static const char *TEXT_HTML = "text/html";


// ===== JSON Composer helping functions =====
// to output JSON formatted data a String is used and appended by new JSON
// elements. Here JSON Strings are always created with comma separators after
// every object. The generated output must be finished by using the jc_sanitize
// function before transmitted.

// Create a property with String value
extern void jc_prop(String &json, const char *key, String value);
// {
//   value.replace("\"", "\\\"");
//   json.concat('\"');
//   json.concat(key);
//   json.concat("\":\"");
//   json.concat(value);
//   json.concat("\","); // comma may be wrong.
// } // jc_prop


// Create a property with int value
extern void jc_prop(String &json, const char *key, int n);
// {
//   jc_prop(json, key, String(n));
// } // jc_prop


extern const char *jc_sanitize(String &json);
// {
//   json.replace(",]", "]");
//   json.replace(",}", "}");
//   return (json.c_str());
// } // jc_sanitize()


/**
 * @brief Construct a new State Handler object
 * @param path The root path of the state ressources.
 * @param board reference to the board.
 */
BoardHandler::BoardHandler(Board *board)
{
  // LOGGER_RAW("BoardHandler:init: %s", path);
  _path = SVC_BOARD;
  _board = board;
}

// ===== board specific services

// void handleStatus() {}


// reset or reboot the device
void BoardHandler::handleReboot(ESP8266WebServer &server, bool wipe)
{
  server.send(200);
  server.client().stop();
  _board->reboot(wipe);
} // handleReboot()


/**
 * @brief Verify if the method and requestUri can be handles by this module.
 * @param requestMethod current http request method.
 * @param requestUri current url of the request.
 * @return true When the method and requestUri match a state request.
 */
bool BoardHandler::canHandle(HTTPMethod requestMethod, String requestUri)
{
  // LOGGER_RAW("BoardHandler:canHandle(%s)=%u", requestUri.c_str(), ret);
  requestUri.toLowerCase();
  return ((requestMethod == HTTP_GET) && (requestUri.startsWith(SVC_ANY)) &&
          (requestUri != "/$desc.xml"));
} // canHandle


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
  String output;
  const char *output_type = nullptr; // when output_type is set then send output as response. 

  bool ret = true;

  requestUri.toLowerCase();
  output.reserve(512);

  if (requestUri == SVC_REBOOT) {
    // Reboot device
    handleReboot(server, false);

  } else if (requestUri.startsWith(SVC_RESET)) {
    // Reset network parameters and reboot
    handleReboot(server, true);

  } else if (requestUri.startsWith(SVC_SYSINFO)) {
    output = "{";
    jc_prop(output, "devicename", _board->deviceName);
    jc_prop(output, "build", __DATE__);
    jc_prop(output, "free heap", ESP.getFreeHeap());

    jc_prop(output, "flash-size", ESP.getFlashChipSize());
    // jc_prop(output, "flash-real-size", ESP.getFlashChipRealSize());

    FSInfo fs_info;
    SPIFFS.info(fs_info);
    jc_prop(output, "fs-totalBytes", fs_info.totalBytes);
    jc_prop(output, "fs-usedBytes", fs_info.usedBytes);

    // WIFI info
    jc_prop(output, "ssid", WiFi.SSID());
    output += "}";
    jc_sanitize(output);
    output_type = TEXT_JSON;

  } else if (requestUri.startsWith(SVC_ELEMENTS)) {
    // List all registered Elements
    ElementRegistry::list(output);
    output_type = TEXT_JSON;

  } else if (requestUri.startsWith(SVC_UPLOAD)) {
    // Bulk File Upload UI.
    output = uploadContent;
    output_type = TEXT_HTML;

  } else if (requestUri.startsWith(SVC_LISTFILES)) {
    // List files in filesystem
    output = "[";
    Dir dir = SPIFFS.openDir("/");

    while (dir.next()) {
      output += "{";
      jc_prop(output, "type", "file");
      jc_prop(output, "name", dir.fileName());
      jc_prop(output, "size", dir.fileSize());
      output += "},";
    } // while
    output += "]";
    jc_sanitize(output);
    output_type = TEXT_JSON;

  } else if (requestUri.startsWith(SVC_BOARD)) {

    // everything behind  "/$board/" is used to address a specific element
    String eId(requestUri.substring(8));

    // LOGGER_RAW("eId(%s)", eId.c_str());
    // LOGGER_RAW(" args=%d", server.args());

    if (server.args() == 0) {
      // get status of all or the specified element
      _board->getState(output, eId);

      server.sendHeader("Cache-control", "NO-CACHE");
      // DEBUG_LOG("  ret:%s\n", output.c_str());

    } else {
      // send action to the specified element
      _board->setState(eId, server.argName(0), server.arg(0));
    } // if
      output_type = TEXT_JSON;

  } else {
    ret = false;
  }

  if (output_type)
    server.send(200, output_type, output);

  return (ret);
} // handle()

// End.