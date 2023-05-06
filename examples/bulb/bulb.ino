/**
 * @file bulb.ino
 * @brief Minimal Sketch that uses the HomeDing Library to implement WiFi bulbs
 * based on the ESP8266 Processor.
 *
 * The use-case covered with this Sketch is to compile a small firmware
 * with the elements for a devices with 1MByte flash without displays like
 * * bulbs
 * * led stripe controllers with single color
 * * led stripe controllers with neopixel
 *
 * Compile with
 * * Board: Generic ESP8266 module
 * * Flash Size: 1M (FS:128kb, OTA:~438KB)
 * * Flash Mode: DOUT
 * * Debug Port: "Disabled"
 * * Debug Level: "None"
 * * MMU: 32+32 balanced
 * * SSL Support: Basic SSL ciphers (lower ROM use)
 * "board": "esp8266:esp8266:generic",
 * "configuration": "xtal=80,vt=flash,exception=disabled,stacksmash=disabled,ssl=basic,mmu=3232,non32xfer=fast,ResetMethod=nodemcu,CrystalFreq=26,FlashFreq=40,FlashMode=dout,eesz=1M128,led=2,sdk=nonosdk_190703,ip=lm2f,dbg=Disabled,lvl=None____,wipe=none,baud=921600",
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
 * * 02.05.2023 bulb Example created from minimal sketch.
 */

// ===== HomeDing Configuration : Enable Elements for the firmware

#define HOMEDING_REGISTER 1

// Enable the Core Elements of the HomeDing Library
#define HOMEDING_INCLUDE_SYSTEM
#define HOMEDING_INCLUDE_REMOTE

// Enable some Core Elements for small devices
// This collection may be used in e.g. remote controllable plugs.
#define HOMEDING_INCLUDE_Switch
#define HOMEDING_INCLUDE_RTCSTATE
#define HOMEDING_INCLUDE_SCENE

// Enable Elements for LIGHT control
#define HOMEDING_INCLUDE_COLOR
#define HOMEDING_INCLUDE_LIGHT
#define HOMEDING_INCLUDE_NEOPIXEL
#define HOMEDING_INCLUDE_MY9291

#include <Arduino.h>
#include <HomeDing.h>

#include <FS.h>        // File System for Web Server Files
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

  // ----- setup the platform with webserver and file system -----

  homeding.init(&server, &LittleFS, "bulb");

  // ----- adding web server handlers -----

  // Builtin Files
  server.addHandler(new BuiltinHandler(&homeding));

  // Board status and actions
  server.addHandler(new BoardHandler(&homeding));

  // UPLOAD and DELETE of static files in the file system.
  server.addHandler(new FileServerHandler(&homeding));

  LOGGER_INFO("setup done");
}  // setup


// handle all give time to all Elements and active components.
void loop(void) {
  server.handleClient();
  homeding.loop();
}  // loop()


// end.
