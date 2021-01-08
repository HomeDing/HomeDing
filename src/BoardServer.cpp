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
#include <Board.h>
#include <Element.h>

#include <BoardServer.h>
#include <ElementRegistry.h>

#include "upload.h"
#include <FS.h>

#include <MicroJsonComposer.h>

#define SVC_ANY "/$"

// integrated htm files
#define PAGE_SETUP "/$setup"
#define PAGE_UPDATE "/$update"
#define SVC_UPLOAD "/$upload"

// services
#define SVC_REBOOT "/$reboot"
#define SVC_RESET "/$reset"
#define SVC_RESETALL "/$resetall"
#define SVC_SYSINFO "/$sysinfo"

#define SVC_ELEMENTS "/$elements"
#define SVC_LISTFILES "/$list"

#define SVC_BOARD "/$board"

/**
 * @brief Construct a new State Handler object
 * @param path The root path of the state ressources.
 * @param board reference to the board.
 */
BoardHandler::BoardHandler(Board *board)
{
  // LOGGER_RAW("BoardHandler:init: %s", path);
  _board = board;
}

// ===== board specific services

// void handleStatus() {}

void BoardHandler::handleConnect(ESP8266WebServer &server)
{
  unsigned long connectTimeout =
      millis() + (60 * 1000); // TODO: make configurable
  server.send(200);

  if (server.hasArg("n")) {
    const char *netName = server.arg("n").c_str();
    const char *netPass = server.arg("p").c_str();

    _board->displayInfo("setup network", netName);
    WiFi.mode(WIFI_STA);
    WiFi.begin(netName, netPass);

  } else if (server.hasArg("wps")) {
    // TODO: start using wps
    // WiFi.beginWPSConfig();
  } // if

  while (connectTimeout > millis()) {
    delay(100);
    wl_status_t wifi_status = WiFi.status();

    if (wifi_status == WL_CONNECTED) {
      _board->displayInfo("ok.");
      break;
    } else if ((wifi_status == WL_NO_SSID_AVAIL) || (wifi_status == WL_CONNECT_FAILED)) {
      _board->displayInfo("failed.");
      WiFi.disconnect(true);
      break;
    } // if
  } // while
  delay(800);
  _board->reboot(false);
} // handleConnect()


// Return list of local networks.
void BoardHandler::handleScan(ESP8266WebServer &server)
{
  int8_t scanState = WiFi.scanComplete();
  if (scanState == WIFI_SCAN_FAILED) {
    // restart a scan
    WiFi.scanNetworks(true);
    server.send(200);
  } else if (scanState == WIFI_SCAN_RUNNING) {
    server.send(200);
  } else {
    // return scan result
    MicroJsonComposer jc;
    jc.openArray();

    for (int i = 0; i < scanState; i++) {
      jc.openObject();
      jc.addProperty("id", WiFi.SSID(i));
      jc.addProperty("rssi", WiFi.RSSI(i));
      jc.addProperty("open", WiFi.encryptionType(i) == ENC_TYPE_NONE);
      jc.closeObject();
    }
    jc.closeArray();
    server.send(200, TEXT_JSON, jc.stringify());
    WiFi.scanDelete();
  }
} // handleScan()

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
  return (requestUri.startsWith(SVC_ANY));
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
  bool unSafeMode = !_board->isSafeMode;

  requestUri.toLowerCase();
  output.reserve(512);

  if (requestUri.startsWith(SVC_BOARD)) {
    // most common request
    // everything behind  "/$board/" is used to address a specific element
    // LOGGER_JUSTINFO("handle(%s)", requestUri.c_str());

    String eId(requestUri.substring(8));

    if (server.args() == 0) {
      // get status of all or the specified element
      _board->getState(output, eId);

      server.sendHeader("Cache-control", "NO-CACHE");
      // LOGGER_JUSTINFO("  ret:%s\n", output.c_str());

    } else {
      // send action to the specified element
      String action(eId);
      action.concat('?');
      action.concat(server.argName(0));
      action.concat('=');
      action.concat(server.arg(0));
      // LOGGER_JUSTINFO(" net-action: %s", action.c_str());
      _board->dispatch(action.c_str(), nullptr);
      // _board->setState(eId, server.argName(0), server.arg(0));
    } // if
    output_type = TEXT_JSON;

  } else if (requestUri.startsWith(SVC_SYSINFO)) {
    unsigned long now = millis();
    MicroJsonComposer jc;

    jc.openObject();
    jc.addProperty("devicename", _board->deviceName);
    jc.addProperty("build", __DATE__);
    jc.addProperty("freeHeap", ESP.getFreeHeap());

    jc.addProperty("flashSize", ESP.getFlashChipSize());
    // jc.addProperty("flash-real-size", ESP.getFlashChipRealSize());

    FSInfo fs_info;
    SPIFFS.info(fs_info);
    jc.addProperty("fsTotalBytes", fs_info.totalBytes);
    jc.addProperty("fsUsedBytes", fs_info.usedBytes);
    jc.addProperty("safemode", _board->isSafeMode ? "true" : "false");
    jc.addProperty("upTime", now / 1000);

    // WIFI info
    jc.addProperty("ssid", WiFi.SSID());

    jc.closeObject();
    output = jc.stringify();
    output_type = TEXT_JSON;

    // ===== these actions are only in non-safemode
  } else if (unSafeMode && (requestUri.startsWith(SVC_RESETALL))) {
    // Reset SPIFFS, network parameters and reboot
    SPIFFS.format();
    handleReboot(server, true);

  } else if (unSafeMode && (requestUri.startsWith(SVC_RESET))) {
    // Reset network parameters and reboot
    handleReboot(server, true);

  } else if (unSafeMode && (requestUri.startsWith(PAGE_SETUP))) {
    // Network Config Page
    output = FPSTR(setupContent);
    output_type = TEXT_HTML;
             
  } else if (unSafeMode && (requestUri.startsWith(PAGE_UPDATE) || requestUri.startsWith("/$boot"))) {
    // Bootstrap page
    output = FPSTR(updateContent);
    output_type = TEXT_HTML;

  } else if (unSafeMode && (requestUri.startsWith(SVC_UPLOAD))) {
    // Bulk File Upload UI.
    output = FPSTR(uploadContent);
    output_type = TEXT_HTML;

  } else if (unSafeMode && (requestUri.startsWith(SVC_LISTFILES))) {
    // List files in filesystem
    MicroJsonComposer jc;
    Dir dir = SPIFFS.openDir("/");

    jc.openArray();
    while (dir.next()) {
      jc.openObject();
      jc.addProperty("type", "file");
      jc.addProperty("name", dir.fileName());
      jc.addProperty("size", dir.fileSize());
      jc.closeObject();
    } // while
    jc.closeArray();
    output = jc.stringify();
    output_type = TEXT_JSON;

  } else if (unSafeMode && (requestUri == "/$scan")) {
    handleScan(server);

  } else if (unSafeMode && (requestUri == "/$connect")) {
    handleConnect(server);

  } else if (requestUri == SVC_REBOOT) {
    // Reboot device
    handleReboot(server);

  } else if (unSafeMode && (requestUri.startsWith(SVC_ELEMENTS))) {
    // List all registered Elements
    ElementRegistry::list(output);
    output_type = TEXT_JSON;

  } else {
    ret = false;
  }

  if (output_type)
    server.send(200, output_type, output);

  return (ret);
} // handle()

// End.