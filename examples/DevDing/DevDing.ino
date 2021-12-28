/**
 * @file DevDing.ino
 * @brief Experimental sketch that uses the HomeDing Library to implement Things attached the
 * Internet.
 * This sketch is also used as an incubator project for new and unfinished elements.
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

// #define DBG_GDB // start with debugger
#define DBG_TRACE // trace level for all elements
#define NET_DEBUG // show network event in output

#ifdef DBG_GDB
#include <GDBStub.h>
#endif

// ===== HomeDing Configuration : Enable Elements for the firmware

#define HOMEDING_REGISTER 1

// Enable the Core Elements of the HomeDing Library
#define HOMEDING_INCLUDE_CORE

// Enable some Sensor Elements
#define HOMEDING_INCLUDE_DHT
#define HOMEDING_INCLUDE_BME680
#define HOMEDING_INCLUDE_DS18B20
#define HOMEDING_INCLUDE_SHT20 // + 1176 bytes

// The PMS uses SoftwareSerial Library that requires more IRAM.
// When using, please switch the MMU: Options to give more IRAM
// #define HOMEDING_INCLUDE_PMS

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
#define HOMEDING_INCLUDE_TM1637

// Enable Elements for LIGHT control
#define HOMEDING_INCLUDE_COLOR
#define HOMEDING_INCLUDE_LIGHT
#define HOMEDING_INCLUDE_NEOPIXEL
#define HOMEDING_INCLUDE_MY9291

#define HOMEDING_INCLUDE_WEATHERFEED

#include <Arduino.h>
#include <HomeDing.h>

#include <FS.h> // File System for Web Server Files
#include <LittleFS.h> // File System for Web Server Files

#include <BuiltinHandler.h> // Serve Built-in files
#include <BoardServer.h> // Web Server Middleware for Elements
#include <FileServer.h> // Web Server Middleware for UI


// ===== WLAN credentials =====

#include "secrets.h"

// WebServer on port 80 to reach Web UI and services
WebServer server(80);

// HomeDing core functionality
Board mainBoard;


// ===== implement =====

/**
 * Setup all components and Serial debugging helpers
 */
void setup(void) {
  Serial.begin(115200);
#ifdef DBG_GDB
  gdbstub_init();
#endif

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

  // ----- setup the platform with webserver and file system -----

  // LittleFS is the default filesystem
  mainBoard.init(&server, &LittleFS, __FILE__);

  // ----- adding web server handlers -----

  // Builtin Files
  server.addHandler(new BuiltinHandler(&mainBoard));

  // Board status and actions
  server.addHandler(new BoardHandler(&mainBoard));

  // UPLOAD and DELETE of static files in the file system.
  server.addHandler(new FileServerHandler(*mainBoard.fileSystem, &mainBoard));

  LOGGER_INFO("setup done.");
} // setup


// handle all give time to all Elements and active components.
void loop(void) {
  server.handleClient();
  mainBoard.loop();
} // loop()


// end.
