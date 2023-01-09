/**
 * @file The BigDisplay.ino
 * @brief The BigDisplay Sketch that uses the HomeDing Library to implement Things attached the
 * Internet.
 * This example includes a driver for a 800*600 px RGB panel that works with the planel DMA interface of a ESP32-S3 module.
 * All default from the standard sketch are included by default.
 *
 * Compile with
 * * Board: ESP32S3 Dev Module
 * * Flash Size: 8MB (64Mb)
 * * Partition Scheme: 8M with spoffs (3MB APP/1.5MB SPIFFS)
 * * Core Debug Level: "None"
 *
 * There is full featured WebUI from the standard example can be used.
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
 * * 01.01.2021 created from standard sketch.
*/ 
// ===== HomeDing Configuration : Enable Elements for the firmware

#define HOMEDING_REGISTER 1

// Enable the following element groups of the HomeDing Library
#define HOMEDING_INCLUDE_SYSTEM
#define HOMEDING_INCLUDE_CORE
#define HOMEDING_INCLUDE_FULL_SYSTEM

// Enable some Common Sensor Elements
#define HOMEDING_INCLUDE_DHT
#define HOMEDING_INCLUDE_DALLAS

// Enable / Disable some more Sensor Elements upon requirement
#define HOMEDING_INCLUDE_AM2320
#define HOMEDING_INCLUDE_SHT20
#define HOMEDING_INCLUDE_AHT20
#define HOMEDING_INCLUDE_BME680
#define HOMEDING_INCLUDE_BH1750

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
// eneble these lines to get more displays supported
// #define HOMEDING_INCLUDE_DISPLAYST7789
// #define HOMEDING_INCLUDE_DISPLAYMAX7219
// #define HOMEDING_INCLUDE_TM1637

// Enable Elements for LIGHT control
#define HOMEDING_INCLUDE_COLOR
#define HOMEDING_INCLUDE_LIGHT
#define HOMEDING_INCLUDE_NEOPIXEL
#define HOMEDING_INCLUDE_MY9291

// Network Services
#define HOMEDING_INCLUDE_MQTT
#define HOMEDING_INCLUDE_WEATHERFEED

#include <Arduino.h>
#include <HomeDing.h>

#include <FS.h>        // File System for Web Server Files
#include <LittleFS.h>  // File System for Web Server Files

#include <BuiltinHandler.h>  // Serve Built-in files
#include <BoardServer.h>     // Web Server Middleware for Elements
#include <FileServer.h>      // Web Server Middleware for UI

// Extra Display
#include "GFX_ESP32Panel.h"
#include "DisplayPanelElement.h"


// ===== WLAN credentials =====

// #include "secrets.h"
const char *ssid = "KHMH";
const char *passPhrase = "hk-2012FD2926";

// WebServer on port 80 to reach Web UI and services
WebServer server(80);

// ===== implement =====

/**
 * Setup all components and Serial debugging helpers
 */
void setup(void) {
  delay(3000);
  Serial.begin(115200);

  Serial.setDebugOutput(false);

  // ----- setup the platform with webserver and file system -----

  homeding.init(&server, &LittleFS, "BigDisplay");

  // ----- adding web server handlers -----

  // Builtin Files
  server.addHandler(new BuiltinHandler(&homeding));

  // Board status and actions
  server.addHandler(new BoardHandler(&homeding));

  // UPLOAD and DELETE of static files in the file system.
  server.addHandler(new FileServerHandler(*homeding.fileSystem, &homeding));

  // Serial.println("Setup Backlight:");
  // pinMode(2, OUTPUT);
  // digitalWrite(2, HIGH);

  LOGGER_INFO("setup done.");
}  // setup


// handle all give time to all Elements and active components.
void loop(void) {
  server.handleClient();
  homeding.loop();
}  // loop()


// end.
