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
 */

#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#include <FS.h>

// =====

#define LOGGER_MODULE "main"

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

// extern "C" {
// #include "user_interface.h"
// }

// ===== WLAN credentials =====

#include "secrets.h"

// need a WebServer
ESP8266WebServer server(80);

// ===== application state variables =====

char devicename[32]; // name of this device on the network
Board mainBoard;

// ===== implement =====

// Send out a JSON object with all files in dir
void handleFileList()
{
  LOGGER_TRACE("handleFileList()");

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

wl_status_t net_connect()
{
  // connect to a same network as before ?
  wl_status_t wifi_status = WiFi.status();

#if NET_DEBUG
  LOGGER_TRACE("wifi status=(%d)", wifi_status);
  if (WiFi.getAutoConnect()) {
    Serial.printf("autoconnect mode enabled.");
  }
#endif

  if (WiFi.getAutoConnect() && WiFi.SSID().equalsIgnoreCase(ssid) &&
      WiFi.psk().equalsIgnoreCase(password)) {
#if NET_DEBUG
    LOGGER_TRACE("same network");
#endif

    if (wifi_status != WL_CONNECTED) {
      LOGGER_TRACE("start reconnect");
      WiFi.mode(WIFI_STA);
      WiFi.begin();
    } // if
  } else {
    LOGGER_TRACE("start new STA");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
  } // if

  // wait max. 30 seconds for connecting
  unsigned long maxTime = millis() + (30 * 1000);

  while (1) {
    wifi_status = WiFi.status();
#if NET_DEBUG
    Serial.printf("(%d).", wifi_status);
#else
    Serial.printf(".");
#endif

    if ((wifi_status == WL_CONNECTED) || (wifi_status == WL_NO_SSID_AVAIL) ||
        (wifi_status == WL_CONNECT_FAILED) || (millis() >= maxTime))
      break; // exit this loop
    delay(100);
  } // while

  Serial.printf(".\n");
  LOGGER_TRACE("net_connect returns(%d)", wifi_status);
  return (wifi_status);
} // net_connect


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

  // ----- setup the file system and load configuration -----
  SPIFFS.begin();
  mainBoard.init(&server);

  // load all config files and create+start elements
  mainBoard.addElements();
  mainBoard.start();

  // Enable the next line to start detailed tracing
  Logger::logger_level = LOGGER_LEVEL_TRACE;

  LOGGER_TRACE("Elements started.\n");

  // ----- setup Display -----
  display = mainBoard.display;
  if (display) {
    display->drawText(0, 0, 0, "HomeDing...");
    display->flush();
    delay(100);
  } // if

  // ----- setup Board and Elements-----

  Element *deviceElement = mainBoard.getElement("device");

  if (deviceElement) {
    strncpy(devicename, deviceElement->get("name"), sizeof(devicename));
    LOGGER_TRACE(" devicename: %s.", devicename);
  } else {
    strncpy(devicename, "homeding", sizeof(devicename));
  } // if

  // ----- setup Web Server -----

  LOGGER_INFO("Starting network...");

  // set device hostname as soon as possible from the device name
  // LOGGER_INFO("hostname=%s", WiFi.hostname().c_str());
  // LOGGER_INFO("devicename=%s", devicename);
  WiFi.hostname(devicename);

  wl_status_t wifi_status = net_connect();

  if (wifi_status != WL_CONNECTED) {
    delay(10000);
    ESP.restart();
  }

  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);

  LOGGER_INFO("Connected to: %s", WiFi.SSID().c_str());
  LOGGER_INFO("  as: %s", WiFi.hostname().c_str());

  String ipStr = WiFi.localIP().toString();
  LOGGER_INFO("  IP: %s", ipStr.c_str());
  LOGGER_INFO(" MAC: %s", WiFi.macAddress().c_str());

  if (display) {
    display->clear();
    display->drawText(0, 0, 0, devicename);
    display->drawText(0, display->lineHeight, 0, ipStr.c_str());
    display->flush();
    delay(1000);
  } // if

#if NET_DEBUG
  WiFi.printDiag(Serial);
#endif

  // ----- adding web server handlers -----

  // redirect to index.htm of only domain name is given.
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Location", "/index.htm", true);
    LOGGER_TRACE("Redirect...");
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
    json += " 'fs-usedBytes':" + String(fs_info.usedBytes) + "\n";

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

  // Data and Commands exchange
  // https://gist.github.com/igrr/0da0c4adc7588d9bd911
  // https://www.intertech.com/Blog/iot-with-an-esp8266-part-2-arduino-sketch/
  // https://learn.adafruit.com/esp8266-temperature-slash-humidity-webserver/code

  server.addHandler(new BoardHandler("/$board", mainBoard));

  server.on("/$reboot", HTTP_GET, []() {
    LOGGER_INFO("rebooting...");
    server.send(200, "text/plain", "");
    delay(500);
    ESP.reset();
  });

  server.addHandler(new FileServerHandler(SPIFFS, "/", "NO-CACHE"));
  server.begin();

  LOGGER_TRACE("Server started.\n");

  if (display) {
    display->clear();
    display->flush();
  } // if

} // setup


// handle all give time to all Elements and active components.
void loop(void)
{
  server.handleClient();
  mainBoard.loop();
} // loop()


// end.
