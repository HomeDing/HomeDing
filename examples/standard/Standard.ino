/**
 * @file Standard.ino
 * @brief Standard Sketch that uses the HomeDing Library to implement Things attached the
 * Internet.
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD 3-Clause style license,
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino
 * and https://homeding.github.io/index.htm#page=/examples/develop.md.
 *
 * Changelog:
 * * 12.11.2019 Standard Example created from development sketch.
 * * 15.04.2020 fixed library dependencies.
 */

// ----- activatable debug options

// ===== HomeDing Configuration : Enable Elements for the firmware

#define HOMEDING_REGISTER 1

// Enable the Core Elements of the HomeDing Library
#define HOMEDING_INCLUDE_CORE

// Enable some Sensor Elements
#define HOMEDING_INCLUDE_DHT
#define HOMEDING_INCLUDE_BME680
#define HOMEDING_INCLUDE_DS18B20
#define HOMEDING_INCLUDE_SHT20 // + 1176 bytes
#define HOMEDING_INCLUDE_PMS

// Enable some INPUT Elements
#define HOMEDING_INCLUDE_ROTARY
#define HOMEDING_INCLUDE_MENU

// Enable some TIME Elements
#define HOMEDING_INCLUDE_DSTIME

// Enable Elements for Displays
#define HOMEDING_INCLUDE_DISPLAY
#define HOMEDING_INCLUDE_DISPLAYLCD
#define HOMEDING_INCLUDE_DISPLAYSSD1306
#define HOMEDING_INCLUDE_DISPLAYSH1106

// Enable Elements for LIGHT control
#define HOMEDING_INCLUDE_COLOR
#define HOMEDING_INCLUDE_LIGHT
#define HOMEDING_INCLUDE_NEOPIXEL
#define HOMEDING_INCLUDE_MY9291

#define HOMEDING_INCLUDE_WEATHERFEED

// ===== Start Arduino Sketch

#include <Arduino.h>
#include <HomeDing.h>

#include <FS.h> // File System for Web Server Files

#include <BoardServer.h> // Web Server Middleware for Elements
#include <FileServer.h> // Web Server Middleware for UI 


// ===== define full functional Web UI with 4MByte Flash devices

#define SETUP_URL "/$setup#v02"


// ===== forward declarations

void handleRedirect();
void setup(void);
void loop(void);

static const char respond404[] PROGMEM =
    R"==(<html><head><title>File not found</title></head><body>File not found</body></html>)==";

// ===== WLAN credentials =====

#include "secrets.h"

// need a WebServer
WebServer server(80);

// ===== application state variables =====

Board mainBoard;

// ===== implement =====

void handleRedirect()
{
  LOGGER_RAW("Redirect...");

  String url;
  if (!mainBoard.isCaptiveMode()) {
    url = mainBoard.homepage;
  } else {
    url = "http://";
    url.concat(WiFi.softAPIP().toString()); // mainBoard.deviceName
    url.concat(SETUP_URL);
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

#ifdef NET_DEBUG
  Serial.setDebugOutput(true);
#else
  Serial.setDebugOutput(false);
#endif

  LOGGER_INFO("Device starting...");

  // ----- setup the file system and load configuration -----
  mainBoard.init(&server, &SPIFFS);
  yield();

  // ----- adding web server handlers -----
  // redirect to index.htm when only domain name is given.
  server.on("/", HTTP_GET, handleRedirect);

  // Board status and actions
  server.addHandler(new BoardHandler(&mainBoard));

  // UPLOAD and DELETE of static files in the file system.
  server.addHandler(new FileServerHandler(*mainBoard.fileSystem, "no-cache", &mainBoard));
  // GET static files is added after network connectivity is given.

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
      server.send(404, TEXT_HTML, FPSTR(respond404));
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
