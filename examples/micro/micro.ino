/**
 * @file Micro.ino
 * @brief HomeDing Micro Sketch that uses the HomeDing Library to implement 
 * a firmware that can be used to upload firmware OTA only.
 *
 * This very small firmware is required to flash a firmware to a ESP8266 device
 * with 1 MByte Flash memory only that exceeds half of the remaining ROM after
 * reserving 128 kByte for littleFS based file system.
 * 
 * These devices typically use ESP-01 boards or 1MByte Flash ROMs.
 *
 * Compile with
 * * Board: Generic ESP8266 module
 * * Flash Size: 1M (FS:128kb, OTA:~438KB)
 * * Flash Mode: DOUT
 * * Debug Port: "Disabled"
 * * Debug Level: "None"
 * * MMU: 32+32 balanced
 * "board": "esp8266:esp8266:generic",
 * "configuration": "xtal=80,vt=flash,exception=disabled,stacksmash=disabled,ssl=all,mmu=3232,non32xfer=fast,ResetMethod=nodemcu,CrystalFreq=26,FlashFreq=40,FlashMode=dout,eesz=1M128,led=2,sdk=nonosdk_190703,ip=lm2f,dbg=Disabled,lvl=None____,wipe=none,baud=115200",
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
 * * 22.04.20232 Micro created from minimal sketch.
 */

// ===== HomeDing Configuration : Enable no further Elements for the firmware

#define HOMEDING_REGISTER 1
// only ["device","ota"] are available

#include <Arduino.h>
#include <HomeDing.h>

#include <FS.h>        // File System for Web Server Files
#include <LittleFS.h>  // File System for Web Server Files

#include <BoardServer.h>     // Web Server Middleware for Elements


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
  delay(3000);
  Serial.println("HomeDing micro.");

  // ----- setup the platform with webserver and file system -----

  homeding.init(&server, &LittleFS, "micro");

  // ----- adding Board status and actions only -----

  // Board status and actions
  server.addHandler(new BoardHandler(&homeding));

  Serial.println("running...");
}  // setup


// handle all give time to all Elements and active components.
void loop(void) {
  server.handleClient();
  homeding.loop();
}  // loop()


// end.
