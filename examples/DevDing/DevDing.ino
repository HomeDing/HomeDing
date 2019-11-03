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

#include <GDBStub.h>

#include <Arduino.h>
#include <Board.h>
#include <Element.h>

#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#include <FS.h>

#include <BoardServer.h>
#include <ElementRegistry.h>
#include <FileServer.h>

#define NET_DEBUG 0

// Use the Core Elements of the HomeDing Library
#define HOMEDING_INCLUDE_CORE

// Use some more Elements that need additional libraries
#define HOMEDING_INCLUDE_DHT
#define HOMEDING_INCLUDE_BME680
#define HOMEDING_INCLUDE_DS18B20
#define HOMEDING_INCLUDE_RFSend
#define HOMEDING_INCLUDE_ROTARY
#define HOMEDING_INCLUDE_MENU

#define HOMEDING_INCLUDE_NTPTIME
#define HOMEDING_INCLUDE_DSTIME

// Use some more Elements for Displays
#define HOMEDING_INCLUDE_DISPLAY
#define HOMEDING_INCLUDE_DISPLAYLCD
#define HOMEDING_INCLUDE_DISPLAYSSD1306
#define HOMEDING_INCLUDE_DISPLAYSH1106

#define HOMEDING_INCLUDE_SERIALLINE

#include <HomeDing.h>

const char *respond404 =
    "<html><head><title>File not found</title></head><body>File not "
    "found</body></html>";

// ===== WLAN credentials =====

#include "secrets.h"

static const char *TEXT_PLAIN = "text/plain";
static const char *TEXT_JSON = "text/json";
static const char *TEXT_HTML = "text/html";

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


void handleRedirect()
{
  LOGGER_RAW("Redirect...");

  String url;
  if (mainBoard.boardState < BOARDSTATE_STARTCAPTIVE) {
    url = mainBoard.homepage;
  } else {
    url = String(F("http://")) + WiFi.softAPIP().toString() +
          F("/$setup"); // ; mainBoard.deviceName
  }
  server.sendHeader("Location", url, true);
  server.send(302);
  server.client().stop();
} // handleRedirect()


/**
 * Setup all components and Serial debugging helpers
 */
void setup(void)
{
  Serial.begin(115200);
  gdbstub_init();

#if NET_DEBUG
  Serial.setDebugOutput(true);
#else
  Serial.setDebugOutput(false);
#endif

  // sometimes configuring the logger_level in the configuration is too late. Then patch loglevel here:
  // Logger::logger_level = LOGGER_LEVEL_TRACE;

  LOGGER_INFO("HomeDing Device is starting... (%s)",
              ESP.getResetReason().c_str());

  // ----- setup the file system and load configuration -----
  SPIFFS.begin();
  mainBoard.init(&server);

  // ----- adding web server handlers -----
  // redirect to index.htm when only domain name is given.
  server.on("/", HTTP_GET, handleRedirect);

  // Board status and actions
  server.addHandler(new BoardHandler(&mainBoard));

  // Static files in the file system.
  server.addHandler(new FileServerHandler(SPIFFS, "NO-CACHE"));

  server.onNotFound([]() {
    const char *uri = server.uri().c_str();
    LOGGER_RAW("notFound: %s", uri);

    if ((mainBoard.boardState == BOARDSTATE_RUNCAPTIVE) &&
        ((strcmp(uri, "/connecttest.txt") == 0) ||
         (strcmp(uri, "/redirect") == 0) ||
         (strcmp(uri, "/generate_204") == 0) ||
         (strcmp(uri, "/more.txt") == 0))) {
      handleRedirect();
    } else {
      // standard not found in browser.
      server.send(404, TEXT_HTML, respond404);
    }
  });

  LOGGER_INFO("setup done.");
} // setup


// handle all give time to all Elements and active components.
void loop(void)
{
  server.handleClient();
  mainBoard.loop();
} // loop()


// end.
