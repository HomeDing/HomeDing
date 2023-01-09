/**
 * @file WebRadio.ino
 * @brief This ketch uses the HomeDing Library and a audio library to implement a WebRadio for streaming files
 * that can be controlled using a Web Interface.
 *
 * This example compiles a customized firmware from the HomeDing library
 * that supports a color graphic display, some elements to control parameters.
 * It runs only on a ESP32 Wrover based board with PSRAM and with I2S Audio support (DAC Converter).
 *
 * Compile with:
 * 
 * * Board: ESP32 Wrover Kit
 * * PSRAM: Enabled
 * * Partition Scheme: 4MB (1.2MB APP / 1.5MB SPIFS)
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
 * * 01.11.2022 Example created.
 */

// ----- activatable debug options

#define DBG_TRACE  // trace level for all elements

// ===== HomeDing Configuration : Enable Elements for the firmware

#define HOMEDING_REGISTER 1

// Enable the following element groups of the HomeDing Library
#define HOMEDING_INCLUDE_SYSTEM
#define HOMEDING_INCLUDE_CORE
#define HOMEDING_INCLUDE_FULL_SYSTEM

// Enable some INPUT Elements
#define HOMEDING_INCLUDE_ROTARY
#define HOMEDING_INCLUDE_MENU

// Enable Elements for Displays
#define HOMEDING_INCLUDE_DISPLAY
// #define HOMEDING_INCLUDE_DISPLAYLCD
// #define HOMEDING_INCLUDE_DISPLAYSSD1306
// #define HOMEDING_INCLUDE_DISPLAYSH1106
#define HOMEDING_INCLUDE_DISPLAYST7789
// #define HOMEDING_INCLUDE_DISPLAYST7735
// #define HOMEDING_INCLUDE_DISPLAYMAX7219

// Network Services
// #define HOMEDING_INCLUDE_MQTT
// #define HOMEDING_INCLUDE_WEATHERFEED

#include <Arduino.h>
#include <HomeDing.h>

#include <FS.h>
#include <LittleFS.h>  // File System for Web Server Files

#include <BuiltinHandler.h>  // Serve Built-in files
#include <BoardServer.h>     // Web Server Middleware for Elements
#include <FileServer.h>      // Web Server Middleware for UI


// ===== WLAN credentials =====

#include "secrets.h"

// WebServer on port 80 to reach Web UI and services
WebServer server(80);

// ===== implement =====

/**
 * Setup all components and Serial debugging helpers
 */
void setup(void) {
  Serial.begin(115200);

#ifdef DBG_TRACE
  // wait so the serial monitor can capture all output.
  delay(3000);
  Serial.println();
  // sometimes configuring the logger_level in the configuration is too late. Then patch loglevel here:
  Logger::logger_level = LOGGER_LEVEL_TRACE;
#endif

  // ----- setup the platform with webserver and file system -----

  homeding.init(&server, &LittleFS, "WebRadio");

  // ----- adding web server handlers -----

  // Builtin Files
  server.addHandler(new BuiltinHandler(&homeding));

  // Board status and actions
  server.addHandler(new BoardHandler(&homeding));

  // UPLOAD and DELETE of static files in the file system.
  server.addHandler(new FileServerHandler(*homeding.fileSystem, &homeding));

  LOGGER_INFO("setup done.");
}  // setup


// handle all give time to all Elements and active components.
void loop(void) {
  server.handleClient();
  homeding.loop();
}  // loop()


// end.
