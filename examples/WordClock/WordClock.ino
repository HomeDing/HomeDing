/**
 * @file WordClock.ino
 * @brief Example sketch for the HomeDing Library to implement a word clock based on network time.
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
 * * 21.11.2020 Example created.
 */

// ----- activatable debug options

#define DBG_TRACE // trace level for all elements
// #define NET_DEBUG // show network event in output
#define HOMEDING_REGISTER 1

// Enable the Core Elements of the HomeDing Library
#define HOMEDING_INCLUDE_CORE

// The Elements required from the standard set can be minimal:

// Network Time is the time source.
#define HOMEDING_INCLUDE_NTPTIME

// Using a MAX7219 driver chip.
#define HOMEDING_INCLUDE_MAX7219

// The WordClock Element in WordClock.cpp is included automatically, because it is in the sketch folder.

#include <Arduino.h>
#include <HomeDing.h>

#include <FS.h> // File System for Web Server Files

#include <BoardServer.h> // Web Server Middleware for Elements
#include <FileServer.h>  // Web Server Middleware for UI

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
#ifdef DBG_TRACE
  delay(3000);
  // sometimes configuring the logger_level in the configuration is too late. Then patch loglevel here:
  Logger::logger_level = LOGGER_LEVEL_TRACE;
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
