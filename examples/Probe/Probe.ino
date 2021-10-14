/**
 * @file Probe.ino
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
 * * 15.06.2021 usable with esp8266 board manager version >= 3.0.0
 */

// ----- activatable debug options

// #define NET_DEBUG

// ===== HomeDing Configuration : Enable Elements for the firmware

#define HOMEDING_REGISTER 1

// Enable the Core Elements of the HomeDing Library
#define HOMEDING_INCLUDE_CORE

// Enable Elements for Displays
#define HOMEDING_INCLUDE_DISPLAY
#define HOMEDING_INCLUDE_DISPLAYLCD
#define HOMEDING_INCLUDE_DISPLAYSSD1306
#define HOMEDING_INCLUDE_DISPLAYSH1106

// The local elements INA219Element and INA226Element
// are addded automatically as they are inside the example folder.


// ===== Start Arduino Sketch

#include <Arduino.h>
#include <HomeDing.h>

#include <FS.h> // File System for Web Server Files
#include <LittleFS.h> // File System for Web Server Files

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

  LOGGER_INFO("Device starting...");

  // ----- setup the platform with webserver and file system -----
  filesys = &LittleFS; // now LittleFS is the default filesystem

  mainBoard.init(&server, filesys);
  hd_yield();

  // ----- adding web server handlers -----

  // Board status and actions
  server.addHandler(new BoardHandler(&mainBoard));

  // UPLOAD and DELETE of static files in the file system.
  server.addHandler(new FileServerHandler(*mainBoard.fileSystem, &mainBoard));
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
      server.send(404, TEXT_HTML, FPSTR(respond404));
    }
  });

  LOGGER_INFO("setup done.");
} // setup


// handle all give time to all Elements and active components.
void loop(void) {
  server.handleClient();
  mainBoard.loop();
} // loop()


// end.