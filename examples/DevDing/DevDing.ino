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
#include <ElementRegistry.h>
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
#define HOMEDING_INCLUDE_DISPLAYSH1106

#include "upload.h"
#include <HomeDing.h>

// ===== WLAN credentials =====

#include "secrets.h"

static const char *TEXT_PLAIN = "text/plain";
static const char *TEXT_HTML = "text/html";
static const char *TEXT_JSON = "text/json";

// need a WebServer
ESP8266WebServer server(80);

// ===== application state variables =====

Board mainBoard;

// ===== implement =====

// ===== JSON Composer helping functions =====
// to output JSON formatted data a String is used and appended by new JSON
// elements. Here JSON Strings are always created with comma separators after
// every object. The generated output must be finished by using the jc_sanitize
// function before transmitted.

// Create a property with String value
void jc_prop(String &json, const char *key, String value)
{
  value.replace("\"", "\\\"");
  json.concat('\"');
  json.concat(key);
  json.concat("\":\"");
  json.concat(value);
  json.concat("\","); // comma may be wrong.
} // jc_prop


// Create a property with int value
void jc_prop(String &json, const char *key, int n)
{
  jc_prop(json, key, String(n));
} // jc_prop


const char *jc_sanitize(String &json)
{
  json.replace(",]", "]");
  json.replace(",}", "}");
  return (json.c_str());
} // jc_sanitize()


// Send out a minimal html file for uploading files.
void handleUploadUI()
{
  server.send(200, TEXT_HTML, uploadContent);
}


void handleRedirect()
{
  BoardState bs = mainBoard.boardState;
  LOGGER_RAW("Redirect...");
  LOGGER_RAW(" BoardState=%d", bs);
  String url;
  if (bs < BOARDSTATE_STARTCAPTIVE) {
    url = F("/index.htm");
  } else {
    url = String(F("http://")) + WiFi.softAPIP().toString() +
          F("/setup.htm"); // ; mainBoard.deviceName
  }
  server.sendHeader("Location", url, true);
  server.send(302, TEXT_PLAIN, "");
  server.client().stop();
} // handleRedirect()


// Return list of local networks.
void handleScan()
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
    String json = "[";
    json.reserve(512);

    for (int i = 0; i < scanState; i++) {
      json += "{";
      jc_prop(json, "id", WiFi.SSID(i));
      jc_prop(json, "rssi", WiFi.RSSI(i));
      jc_prop(json, "open", WiFi.encryptionType(i) == ENC_TYPE_NONE);
      json += "},";
    }
    json += "]";
    server.send(200, TEXT_JSON, jc_sanitize(json));
    WiFi.scanDelete();
  }
} // handleScan()


// Send out a JSON object with all files in dir
void handleFileList()
{
  // LOGGER_RAW("handleFileList()");
  String json;
  json.reserve(512);

  json = "[";
  Dir dir = SPIFFS.openDir("/");

  while (dir.next()) {
    json += "{";
    jc_prop(json, "type", "file");
    jc_prop(json, "name", dir.fileName());
    jc_prop(json, "size", dir.fileSize());
    json += "},";
  } // while
  json += "]";
  server.send(200, TEXT_JSON, jc_sanitize(json));
} // handleFileList


// Send out some device information.
// call
void handleSysInfo()
{
  String json = "{";
  jc_prop(json, "devicename", mainBoard.deviceName);
  jc_prop(json, "build", __DATE__);
  jc_prop(json, "free heap", ESP.getFreeHeap());

  jc_prop(json, "flash-size", ESP.getFlashChipSize());
  jc_prop(json, "flash-real-size", ESP.getFlashChipRealSize());

  FSInfo fs_info;
  SPIFFS.info(fs_info);
  jc_prop(json, "fs-totalBytes", fs_info.totalBytes);
  jc_prop(json, "fs-usedBytes", fs_info.usedBytes);

  // WIFI info
  jc_prop(json, "ssid", WiFi.SSID());
  // jc_prop(json, "bssid", WiFi.BSSIDstr());

  // json += " 'wifi-opmode':" + String(wifi_get_opmode()) + "\n";
  // json += " 'wifi-phymode':" + String(wifi_get_phy_mode()) + "\n";
  // json += " 'wifi-channel':" + String(wifi_get_channel()) + "\n";
  // json += " 'wifi-ap-id':" + String(wifi_station_get_current_ap_id()) +
  // "\n"; json += " 'wifi-status':" +
  // String(wifi_station_get_ct_status()) + "\n";

  json += "}";
  // json.replace('\'', '"');
  server.send(200, TEXT_JSON, jc_sanitize(json));
  json = String();
}


// Send out a minimal html file for uploading files.
void handleElements()
{
  String buffer;
  ElementRegistry::list(buffer);
  server.send(200, TEXT_HTML, buffer);
}

void handleReboot(ESP8266WebServer &server, bool wipe = false)
{
  LOGGER_INFO("rebooting...");
  server.send(200);
  server.client().stop();
  mainBoard.reboot(wipe);
} // handleReboot()


/**
 * Setup all components and Serial debugging helpers
 */
void setup(void)
{
  // unsigned long now = millis();
  DisplayAdapter *display = NULL;

  Serial.begin(115200);

#if NET_DEBUG
  Serial.setDebugOutput(true);
#else
  Serial.setDebugOutput(false);
#endif

  LOGGER_INFO("HomDing Device is starting... (%s), %d", __DATE__,
              ESP.getBootMode());

  // Output of Info and Errors is standard
  Logger::logger_level = LOGGER_LEVEL_ERR;

  // Enable the next line to start detailed tracing
  Logger::logger_level = LOGGER_LEVEL_TRACE;

  // ----- setup the file system and load configuration -----


  SPIFFS.begin();
  mainBoard.init(&server);

  // ----- adding web server handlers -----

  LOGGER_RAW("register handlers.");

  // redirect to index.htm when only domain name is given.
  server.on("/", HTTP_GET, handleRedirect);

  // return some system information
  server.on("/$sysinfo", HTTP_GET, handleSysInfo);
  // list all registered elements.
  server.on("/$elements", HTTP_GET, handleElements);
  server.on("/$reboot", []() { handleReboot(server, false); });
  server.on("/$reset", []() { handleReboot(server, true); });

  // Bulk File Upload UI.
  server.on("/$upload", HTTP_GET, handleUploadUI);

  // Bulk File Upload UI.
  server.on("/$scan", HTTP_GET, handleScan);

  // list files in filesystem
  server.on("/$list", HTTP_GET, handleFileList);


  // Board status and actions
  server.addHandler(new BoardHandler(&mainBoard));

  // Helpers for the captive portal
  server.on("/generate_204", HTTP_GET, handleRedirect);
  server.on("/chat", HTTP_GET, handleRedirect);

  // modify network connection and reboot.
  server.on("/$connect", HTTP_GET, []() {
    unsigned long connectTimeout =
        millis() + (30 * 1000); // TODO: make configurable
    LOGGER_INFO("setup network...");
    server.send(200);

    if (server.hasArg("n")) {
      WiFi.mode(WIFI_STA);
      WiFi.begin(server.arg("n").c_str(), server.arg("p").c_str());
    } else if (server.hasArg("wps")) {
      // TODO: start using wps
      // WiFi.beginWPSConfig();
    } // if

    while (connectTimeout > millis()) {
      delay(500);
      wl_status_t wifi_status = WiFi.status();

      if ((wifi_status == WL_CONNECTED) || (wifi_status == WL_NO_SSID_AVAIL) ||
          (wifi_status == WL_CONNECT_FAILED)) {
        LOGGER_INFO("status = %d", wifi_status);
        break;
      } // if
    } // while
    mainBoard.reboot(false);
  });

  // Static files in the file system.
  server.addHandler(new FileServerHandler(SPIFFS, "/", "NO-CACHE"));

  server.onNotFound([]() {
    LOGGER_RAW("NotFound... %s", server.uri().c_str());
    server.send(404, TEXT_PLAIN, "");
  });


  LOGGER_INFO("sketch setup done.");
} // setup


// handle all give time to all Elements and active components.
void loop(void)
{
  server.handleClient();
  mainBoard.loop();
} // loop()


// end.
