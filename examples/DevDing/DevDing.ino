/**
 * @file DevDing.ino
 * @brief Sketch that uses the HomeDing Library to implement Things attached the
 * Internet.
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
 * * 02.06.2016 created using the FSWebServer sample.
 * * 03.06.2016 ArduinoJson library added.
 * * 05.06.2016 using server.serveStatic.
 * * 05.06.2016 server.sendHeader("Cache-control" , "NO-CACHE"); added in
 *              various places.
 * * 07.06.2016 Timer added
 * * 25.06.2016 handling /all and /list removed.
 * * 02.07.2016 read main_settings from JSON file main_settings.txt
 * * 17.07.2016 config data services added.
 * * 20.08.2016 config of a device name added.
 * * 21.08.2016 reboot using Sleep deep
 * * 23.04.2018 reused to build the HomeDing Library
 * * 27.04.2018 parameter pushing & loading added.
 * * 20.06.2018 SSDP converted to an element
 * * 25.06.2018 ArduinoJson replace by MicroJsonParser. in use only in board.
 * * 22.08.2018 separate network and device configuration.
 * * 18.09.2018 display configuration now in env.json using elements
 * * 10.10.2018 more robust startup.
 * * 10.10.2018 extend sysinfo
 * * 11.10.2018 move network initialization into board loop.
 */

#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#include <FS.h>

#include <Board.h>
#include <BoardServer.h>
#include <FileServer.h>

#define NET_DEBUG 0

// Use the Core Elements of the HomeDing Library
#define HOMEDING_INCLUDE_CORE

// Use some more Elements that need additional libraries
#define HOMEDING_INCLUDE_DHT
#define HOMEDING_INCLUDE_DS18B20
#define HOMEDING_INCLUDE_RFSend
#define HOMEDING_INCLUDE_ROTARY

// Use some more Elements for Displays
#define HOMEDING_INCLUDE_DISPLAYLCD
#define HOMEDING_INCLUDE_DISPLAYSSD1306

#include <HomeDing.h>

// ===== WLAN credentials =====

#include "secrets.h"

// need a WebServer
ESP8266WebServer server(80);

// ===== application state variables =====

Board mainBoard;

// ===== implement =====

// Send out a JSON object with all files in dir
void handleFileList()
{
  LOGGER_RAW("handleFileList()");

  String json;
  json.reserve(512);

  json = "[";
  Dir dir = SPIFFS.openDir("/");

  while (dir.next()) {
    json += "{'type': 'file',";
    json += "'name':'" + dir.fileName() + "',";
    json += "'size':" + String(dir.fileSize());
    json += "},\n";
  } // while

  json += "]";
  json.replace('\'', '"');
  json.replace(",\n]", "\n]");

  server.send(200, "text/json", json);
} // handleFileList


/**
 * Setup all components and Serial debugging helpers
 */
void setup(void)
{
  unsigned long now = millis();
  DisplayAdapter *display = NULL;

  Serial.begin(115200);

#if NET_DEBUG
  Serial.setDebugOutput(true);
#else
  Serial.setDebugOutput(false);
#endif

  LOGGER_INFO("HomDing Device is starting... (%s), %d", __DATE__,
              ESP.getBootMode());

  // Enable the next line to start detailed tracing
  Logger::logger_level = LOGGER_LEVEL_INFO;

  // ----- setup the file system and load configuration -----

  SPIFFS.begin();
  mainBoard.init(&server);

  // ----- adding web server handlers -----

  LOGGER_RAW("register handlers.");

  // redirect to index.htm of only domain name is given.
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Location", "/index.htm", true);
    LOGGER_RAW("Redirect...");
    server.send(301, "text/plain", "");
  });

  // get heap status, analog input value and all GPIO statuses in one json
  // call
  server.on("/$sysinfo", HTTP_GET, []() {
    FSInfo fs_info;

    String json = "{";
    json += " 'build': '" __DATE__ "'\n";
    json += " 'free heap':" + String(ESP.getFreeHeap()) + ",\n";

    json += " 'flash-size':" + String(ESP.getFlashChipSize()) + ",\n";
    json += " 'flash-real-size':" + String(ESP.getFlashChipRealSize()) + ",\n";

    SPIFFS.info(fs_info);
    json += " 'fs-totalBytes':" + String(fs_info.totalBytes) + ",\n";
    json += " 'fs-usedBytes':" + String(fs_info.usedBytes) + ",\n";

    // WIFI info
    json += " 'ssid':'" + WiFi.SSID() + "',\n";
    json += " 'bssid':'" + WiFi.BSSIDstr() + "'\n";

    // json += " 'wifi-opmode':" + String(wifi_get_opmode()) + "\n";
    // json += " 'wifi-phymode':" + String(wifi_get_phy_mode()) + "\n";
    // json += " 'wifi-channel':" + String(wifi_get_channel()) + "\n";
    // json += " 'wifi-ap-id':" + String(wifi_station_get_current_ap_id()) +
    // "\n"; json += " 'wifi-status':" +
    // String(wifi_station_get_connect_status()) + "\n";

    json += "}";
    json.replace('\'', '"');

    server.send(200, "text/json", json);
    json = String();
  });

  // list directory
  server.on("/$list", HTTP_GET, handleFileList);

  server.addHandler(new BoardHandler("/$board", &mainBoard));

  server.on("/$reboot", HTTP_GET, []() {
    LOGGER_INFO("remote rebooting...");
    server.send(200, "text/plain", "");
    delay(500);
    ESP.reset();
  });

  server.addHandler(new FileServerHandler(SPIFFS, "/", "NO-CACHE"));

  LOGGER_INFO("sketch setup done.");
} // setup


// handle all give time to all Elements and active components.
void loop(void)
{
  server.handleClient();
  mainBoard.loop();
} // loop()


// end.
