/**
 * @file tutorial.ino
 * @brief Tutorial sketch that uses the HomeDing Library to implement Things attached the
 * Internet.
 *
 * The tutorial documentation can be found at
 * [How to create and extend a new Element](https://homeding.github.io/steps/newelement.md)
 *
 * Compile with
 * * Board: NodeMCU 1.0
 * * Flash Size: 4M (FS:2MB, OTA:~1019KB)
 * * Debug Port: "Serial"
 * * Debug Level: "None"
 * * MMU: 32+32 balanced
 *
 * There is full featured WebUI available in the data folder.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD 3-Clause style license,
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino
 * and https://homeding.github.io/index.htm#page=/examples/tutorial.md.
 *
 * Changelog:
 * * 25.02.2023 Tutorial Example created from development sketch.
 */

// ----- activatable debug options

#define DBG_TRACE  // trace level for all elements
#define NET_DEBUG  // show network event in output

// ===== HomeDing Configuration : Enable Elements for the firmware

#define HOMEDING_REGISTER 1

// Enable the following element groups of the HomeDing Library
#define HOMEDING_INCLUDE_SYSTEM
#define HOMEDING_INCLUDE_CORE
#define HOMEDING_INCLUDE_FULL_SYSTEM

// Enable some Sensor Elements
#define HOMEDING_INCLUDE_DHT
#define HOMEDING_INCLUDE_AM2320
#define HOMEDING_INCLUDE_SHT20
#define HOMEDING_INCLUDE_AHT20
#define HOMEDING_INCLUDE_DALLAS
#define HOMEDING_INCLUDE_BMP280
#define HOMEDING_INCLUDE_BME680
#define HOMEDING_INCLUDE_BH1750
#define HOMEDING_INCLUDE_SCD4X

// The PMS uses SoftwareSerial Library that requires more IRAM.
// When using, please switch the MMU: Options to give more IRAM
// #define HOMEDING_INCLUDE_PMS

// Enable some INPUT Elements
#define HOMEDING_INCLUDE_ROTARY
#define HOMEDING_INCLUDE_MENU

// Enable some TIME Elements
#define HOMEDING_INCLUDE_DSTIME

// Enable Elements for Displays (+36k program space, 32400 free heap)
#define HOMEDING_INCLUDE_DISPLAY
#define HOMEDING_INCLUDE_DISPLAYLCD
#define HOMEDING_INCLUDE_DISPLAYSSD1306
#define HOMEDING_INCLUDE_DISPLAYSH1106

// Enable simple display Elements
#define HOMEDING_INCLUDE_TM1637
#define HOMEDING_INCLUDE_MAX7219

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

#include <FS.h>
#include <LittleFS.h>  // File System for Web Server Files

#include <BuiltinHandler.h>  // Serve Built-in files
#include <BoardServer.h>     // Web Server Middleware for Elements
#include <FileServer.h>      // Web Server Middleware for UI


// ===== WLAN credentials =====

#include "secrets.h"

// #include "MyElement-01.h"
// #include "MyElement-02.h"

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

#if defined(NET_DEBUG) && defined(ESP8266)
  Serial.setDebugOutput(true);
  // eSTAConnected = WiFi.onStationModeConnected(onSTAConnected);
  static WiFiEventHandler eSTAConnected =
    WiFi.onStationModeConnected([](WiFiEventStationModeConnected e) {
      Serial.printf("WiFi Connected: SSID %s Channel %d\n",
                    e.ssid.c_str(), e.channel);
    });

  static WiFiEventHandler eSTAGotIP =
    WiFi.onStationModeGotIP([](WiFiEventStationModeGotIP e) {
      Serial.printf("WiFi GotIP: localIP %s SubnetMask %s GatewayIP %s\n",
                    e.ip.toString().c_str(), e.mask.toString().c_str(), e.gw.toString().c_str());
    });

  static WiFiEventHandler eSTADisConnected =
    WiFi.onStationModeDisconnected([](WiFiEventStationModeDisconnected e) {
      Serial.printf("WiFi Disconnected: SSID %s Reason %d\n",
                    e.ssid.c_str(), e.reason);
    });

  static WiFiEventHandler h3 = WiFi.onStationModeDHCPTimeout([](void) {
    Serial.println("WiFi Station DHCPTimeout.");
  });

  static WiFiEventHandler h4 = WiFi.onSoftAPModeStationConnected([](const WiFiEventSoftAPModeStationConnected&) {
    Serial.println("WiFi AP Station connected.");
  });

  static WiFiEventHandler h5 = WiFi.onSoftAPModeProbeRequestReceived([](const WiFiEventSoftAPModeProbeRequestReceived&) {
    Serial.println("WiFi AP Station probe.");
  });

  Serial.printf("WiFi.AutoConnect=%d\n", WiFi.getAutoConnect());

#elif defined(NET_DEBUG) && defined(ESP32)
  Serial.setDebugOutput(true);

  // WiFi.onEvent()

#else
  Serial.setDebugOutput(false);
#endif

  // ----- setup the platform with webserver and file system -----

  // LittleFS is the default filesystem
  homeding.init(&server, &LittleFS, "DevDing");

  // ----- adding web server handlers -----

  // Builtin Files
  server.addHandler(new BuiltinHandler(&homeding));

  // Board status and actions
  server.addHandler(new BoardHandler(&homeding));

  // UPLOAD and DELETE of static files in the file system.
  server.addHandler(new FileServerHandler(&homeding));

  // enable initialization line to see MyElement working
  // homeding.add("my/1", new MyElement01());
  // homeding.add("my/2", new MyElement02());

  LOGGER_INFO("setup done");
}  // setup


// handle all give time to all Elements and active components.
void loop(void) {
#ifdef NET_DEBUG
  static wl_status_t lastState = (wl_status_t)100;
  wl_status_t newState = WiFi.status();
  if (newState != lastState) {
    Serial.printf("WiFi status: %d\n", newState);
    lastState = newState;
  }
#endif

  server.handleClient();
  homeding.loop();
}  // loop()


// end.
