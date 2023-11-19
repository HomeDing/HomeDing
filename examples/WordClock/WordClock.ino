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
 * * 15.06.2021 usable with esp8266 board manager version >= 3.0.0
 */

// ----- activatable debug options

#define DBG_TRACE // trace level for all elements
// #define NET_DEBUG // show network event in output
#define HOMEDING_REGISTER 1

// Enable the Core Elements of the HomeDing Library
#define HOMEDING_INCLUDE_CORE

// The Elements required from the standard set can be reduced to the minimal set:

// Network Time is the time source.
#define HOMEDING_INCLUDE_NTPTIME

// Using a MAX7219 driver chip.
#define HOMEDING_INCLUDE_MAX7219

// The WordClock Element in WordClock.cpp is included automatically, because it is in the sketch folder.

#include <Arduino.h>
#include <HomeDing.h>

#include <FS.h> // File System for Web Server Files
#include <LittleFS.h> // File System for Web Server Files

#include <BuiltinHandler.h> // Serve Built-in files
#include <BoardServer.h> // Web Server Middleware for Elements
#include <FileServer.h> // Web Server Middleware for UI


static const char respond404[] PROGMEM =
    "<html><head><title>File not found</title></head><body>File not found</body></html>";

// ===== WLAN credentials =====

#include "secrets.h"

// need a WebServer
WebServer server(80);

// HomeDing core functionality
Board mainBoard;

// Filesystem to be used.
FS *filesys;


// ===== implement =====

/**
 * Setup all components and Serial debugging helpers
 */
void setup(void) {
  Serial.begin(115200);

#ifdef NET_DEBUG
  Serial.setDebugOutput(true);
#else
  Serial.setDebugOutput(false);
#endif

#ifdef DBG_TRACE
  // wait so the serial monitor can capture all output.
  delay(3000);
  // sometimes configuring the logger_level in the configuration is too late. Then patch loglevel here:
  Logger::logger_level = LOGGER_LEVEL_TRACE;
#endif

  LOGGER_INFO("Device (" __FILE__ ") starting...");

  // ----- setup the platform with webserver and file system -----
  filesys = &LittleFS; // LittleFS is the default filesystem

  mainBoard.init(&server, filesys);
  hd_yield();

  // ----- adding web server handlers -----

  // Builtin Files
  server.addHandler(new BuiltinHandler(&mainBoard));

  // Board status and actions
  server.addHandler(new BoardHandler(&mainBoard));

  // UPLOAD and DELETE of static files in the file system.
  server.addHandler(new FileServerHandler(&homeding));
  // GET static files is added after network connectivity is given.

  server.onNotFound([]() {
    const char *uri = server.uri().c_str();
    LOGGER_JUSTINFO("notFound: %s", uri);

    if (mainBoard.isCaptiveMode() && (!filesys->exists(uri))) {
      String url = "http://192.168.4.1/$setup.htm";
      server.sendHeader("Location", url, true);
      server.send(302);

    } else {
      // standard not found in browser.
      server.send(404, "text/html", FPSTR(respond404));
    }
  });

  LOGGER_INFO("setup done");
} // setup


// handle all give time to all Elements and active components.
void loop(void) {
  server.handleClient();
  mainBoard.loop();
} // loop()


// end.
