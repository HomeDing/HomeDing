/**
 * @file Minimal.ino
 * @brief Minimal Sketch that uses the HomeDing Library to implement Things attached the
 * Internet.
 * 
 * The primary reason for this Sketch is to have a small firmware with the elements
 * for a simple switching device without displays.
 * 
 * There is minified WebUI available. 
 * 
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
 * * 12.11.2019 Minimal Example created from development sketch.
 * * 20.12.2019 updated from DevDing example
 * * 07.04.2019 updated from DevDing example, no sensor elements, no elements that need libraries.
 */

// ----- activatable debug options

// #define DBG_TRACE // trace level for all elements
// #define NET_DEBUG // show network event in output

// ===== HomeDing Configuration : Enable Elements for the firmware

#define HOMEDING_REGISTER 1

// Enable the Core Elements of the HomeDing Library
#define HOMEDING_INCLUDE_SYSTEM

// Enable some Core Elements for small devices
// This collection may be used in e.g. remote controllable plugs.
#define HOMEDING_INCLUDE_Value
#define HOMEDING_INCLUDE_Button
#define HOMEDING_INCLUDE_Switch
#define HOMEDING_INCLUDE_DigitalIn
#define HOMEDING_INCLUDE_DigitalOut

#define HOMEDING_INCLUDE_AND
#define HOMEDING_INCLUDE_Map
#define HOMEDING_INCLUDE_REFERENCE
#define HOMEDING_INCLUDE_Timer
#define HOMEDING_INCLUDE_Schedule
#define HOMEDING_INCLUDE_Alarm
#define HOMEDING_INCLUDE_REMOTE


// Enable some Sensor Elements
#define HOMEDING_INCLUDE_DHT
#define HOMEDING_INCLUDE_BL0937

// Enable Elements for LIGHT control
#define HOMEDING_INCLUDE_COLOR
#define HOMEDING_INCLUDE_LIGHT
#define HOMEDING_INCLUDE_NEOPIXEL
#define HOMEDING_INCLUDE_MY9291

#include <Arduino.h>
#include <HomeDing.h>

#include <FS.h> // File System for Web Server Files

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
  delay(3000);
  // sometimes configuring the logger_level in the configuration is too late. Then patch loglevel here:
  Logger::logger_level = LOGGER_LEVEL_TRACE;
#endif

  LOGGER_INFO("Device (" __FILE__ ") starting...");

  // ----- setup the platform with webserver and file system -----

  // filesys = &LittleFS; // now LittleFS is the default filesystem
  filesys = &SPIFFS; // use this line when compiling for SPIFFS 

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
