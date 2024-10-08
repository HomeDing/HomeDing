/**
 * @file DevDing.ino
 * @brief Experimental sketch that uses the HomeDing Library to implement Things attached the
 * Internet.
 * This sketch is also used as an incubator project for new and unfinished elements.
 *
 * The use-case covered with this Sketch is to compile a full featured firmware
 * with the most common and not too execeptional elements for a devices with 4MByte flash like
 * * full featured environment sensors
 * * ...
 * All elements from the minimal sketch are included by default.
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
 * and https://homeding.github.io/index.htm#page=/examples/develop.md.
 *
 * Changelog:
 * * 12.11.2019 Standard Example created from development sketch.
 * * 15.04.2020 fixed library dependencies.
 * * 15.06.2021 usable with esp8266 board manager version >= 3.0.0
 * * 25.02.2023 template and example elements moved into the tutorial example.
 */

// ----- activatable debug options

// #define DBG_GDB // start with debugger
#define DBG_TRACE  // trace level for all elements
#define NET_DEBUG  // show network event in output

#ifdef DBG_GDB
#include <GDBStub.h>
#endif

// ===== HomeDing Configuration : Enable Elements for the firmware

#define HOMEDING_REGISTER 1

// Enable the following element groups of the HomeDing Library
#define HOMEDING_INCLUDE_SYSTEM
#define HOMEDING_INCLUDE_CORE
#define HOMEDING_INCLUDE_FULL_SYSTEM

// Enable some Sensor Elements
// #define HOMEDING_INCLUDE_DHT
// #define HOMEDING_INCLUDE_AM2320
// #define HOMEDING_INCLUDE_SHT20
// #define HOMEDING_INCLUDE_AHT20
// #define HOMEDING_INCLUDE_DALLAS
// #define HOMEDING_INCLUDE_BMP280
// #define HOMEDING_INCLUDE_BME680
// #define HOMEDING_INCLUDE_BH1750
// #define HOMEDING_INCLUDE_SCD4X

// #define HOMEDING_INCLUDE_TONE

// The PMS uses SoftwareSerial Library that requires more IRAM.
// When using, please switch the MMU: Options to give more IRAM
// #define HOMEDING_INCLUDE_PMS

// Enable some INPUT Elements
#define HOMEDING_INCLUDE_ROTARY
#define HOMEDING_INCLUDE_MENU

// Enable some TIME Elements
#define HOMEDING_INCLUDE_DSTIME

// Enable Elements for Displays (+36k program space, 32400 free heap)
// #define HOMEDING_INCLUDE_DISPLAY
// #define HOMEDING_INCLUDE_DISPLAYLCD
// #define HOMEDING_INCLUDE_DISPLAYSSD1306
// #define HOMEDING_INCLUDE_DISPLAYSH1106
// #define HOMEDING_INCLUDE_DISPLAYST7789
// #define HOMEDING_INCLUDE_DISPLAYST7796
// #define HOMEDING_INCLUDE_DISPLAYST7735
// #define HOMEDING_INCLUDE_DISPLAYMAX7219

// Enable simple display Elements
// #define HOMEDING_INCLUDE_TM1637
// #define HOMEDING_INCLUDE_MAX7219

// Enable Elements for LIGHT control
#define HOMEDING_INCLUDE_COLOR
#define HOMEDING_INCLUDE_LIGHT
#define HOMEDING_INCLUDE_NEOPIXEL
#define HOMEDING_INCLUDE_APA102
// #define HOMEDING_INCLUDE_MY9291

// Network Services
// #define HOMEDING_INCLUDE_MQTT
// #define HOMEDING_INCLUDE_WEATHERFEED
// #define HOMEDING_INCLUDE_SDMMC
// #define HOMEDING_INCLUDE_SD

#include <Arduino.h>
#include <HomeDing.h>

#include <FS.h>
#include <LittleFS.h>  // File System for Web Server Files

#if defined(ESP32)
#include <FFat.h>  // File System for Web Server Files
#endif

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
  fs::FS* fsys = nullptr;
  Serial.begin(115200);
#if 0
  Serial.setTxTimeoutMs(0);
#endif

#if ARDUINO_USB_CDC_ON_BOOT
  Serial.setTxTimeoutMs(0);

#else
  while (!Serial)
    yield();
#endif

#ifdef DBG_GDB
  gdbstub_init();
#endif

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

  static WiFiEventHandler h4 = WiFi.onSoftAPModeStationConnected([](const WiFiEventSoftAPModeStationConnected& event) {
    Serial.println("WiFi AP Station connected.");
  });

  static WiFiEventHandler h5 = WiFi.onSoftAPModeProbeRequestReceived([](const WiFiEventSoftAPModeProbeRequestReceived& event) {
    Serial.println("WiFi AP Station probe.");
  });

  Serial.printf("WiFi.AutoConnect=%d\n", WiFi.getAutoConnect());

#elif defined(NET_DEBUG) && defined(ESP32)
  Serial.setDebugOutput(true);

  // WiFi.onEvent()

  WiFi.onEvent([](arduino_event_id_t event) {
    Serial.println("ARDUINO_EVENT_WIFI_STA_CONNECTED.");
  },
               ARDUINO_EVENT_WIFI_STA_CONNECTED);

  WiFi.onEvent([](arduino_event_id_t event) {
    Serial.println("ARDUINO_EVENT_WIFI_AP_START.");
  },
               ARDUINO_EVENT_WIFI_AP_START);
  WiFi.onEvent([](arduino_event_id_t event) {
    Serial.println("ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED.");
  },
               ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED);
  WiFi.onEvent([](arduino_event_id_t event) {
    Serial.println("ARDUINO_EVENT_WIFI_AP_STACONNECTED.");
  },
               ARDUINO_EVENT_WIFI_AP_STACONNECTED);


  // ARDUINO_EVENT_WIFI_AP_STOP,
  // ARDUINO_EVENT_WIFI_AP_STADISCONNECTED,
  // ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED,
  // ARDUINO_EVENT_WIFI_AP_GOT_IP6,

#else
  Serial.setDebugOutput(false);
#endif

  // ----- setup the platform with webserver and file system -----

#if defined(ESP8266)
  // LittleFS is the default filesystem on ESP8266
  fsys = &LittleFS;

#elif defined(ESP32)
  // ----- check partitions for finding the fileystem type -----
  esp_partition_iterator_t i;

  i = esp_partition_find(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_FAT, nullptr);
  if (i) {
    fsys = &FFat;

  } else {
    i = esp_partition_find(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_SPIFFS, nullptr);
    if (i) {
      fsys = &LittleFS;
    }
  }
  esp_partition_iterator_release(i);
#endif

  homeding.init(&server, fsys, "DevDing");

  // ----- adding web server handlers -----

  // Builtin Files
  server.addHandler(new BuiltinHandler(&homeding));

  // Board status and actions
  server.addHandler(new BoardHandler(&homeding));

  // UPLOAD and DELETE of static files in the file system.
  server.addHandler(new FileServerHandler(&homeding));

  LOGGER_JUSTINFO("setup done");
}  // setup


unsigned long lastPrint = 0;
long memLoss = 0;

// handle all give time to all Elements and active components.
void loop(void) {
  unsigned long now = millis();
  unsigned long mem = ESP.getFreeHeap();

  server.handleClient();
  homeding.loop();

  memLoss = memLoss + ESP.getFreeHeap();
  memLoss = memLoss - mem;

  if (now > lastPrint + 2000) {
    Serial.printf("heap: %ld (%ld)\n", mem, memLoss);
    lastPrint = now;
  }
}  // loop()


// end.
