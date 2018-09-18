/**
 * @file DevDing.ino
 * @brief Sketch that uses the HomeDing Library to implement Things attached the
 * Internet.
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license,
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino.
 *
 * Changelog:
 * * 02.06.2016 created using the FSWebServer sample.
 * * 03.06.2016 ArduinoJson library added.
 * * 05.06.2016 using server.serveStatic.
 * * 05.06.2016 server.sendHeader("Cache-control" , "NO-CACHE"); added in
 *              various places.
 * * 07.06.2016 Timer added
 * * 25.06.2016 handling /all and /list removed.
 * * 02.07.2016 read main_settings from JSON file main_settings.txt
 * * 17.07.2016 config data services added.
 * * 20.08.2016 config of a device name added.
 * * 21.08.2016 reboot using Sleep deep
 * * 23.04.2018 reused to build the HomeDing Library
 * * 27.04.2018 parameter pushing & loading added.
 * * 20.06.2018 SSDP converted to an element
 * * 25.06.2018 ArduinoJson replace by MicroJsonParser. in use only in board.
 * * 22.08.2018 separate network and device configuration.
 */

#ifdef DEBUG_ESP_PORT
#define DEBUG_MSG(...) DEBUG_ESP_PORT.printf(">" __VA_ARGS__)
#define DEBUG_LOG(...) DEBUG_ESP_PORT.printf(">Main:" __VA_ARGS__)
#else
#define DEBUG_MSG(...)
#define DEBUG_LOG(...)
#endif

#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#include <FS.h>

// include the hardware supporting libraries
#include "DisplayAdapterLCD.h"
#include <DisplayAdapterSSD1306.h>

// =====

#define LOGGER_MODULE "main"

#include <Board.h>
#include <BoardServer.h>
#include <FileServer.h>

// Use the Core Elements of the HomeDing Library
#define HOMEDING_INCLUDE_CORE

// Use some more Elements that need additional libraries
#define HOMEDING_INCLUDE_DHT
#define HOMEDING_INCLUDE_DS18B20
#define HOMEDING_INCLUDE_RFSend
#define HOMEDING_INCLUDE_ROTARY

#include <HomeDing.h>

extern "C" {
#include "user_interface.h"
}

// ===== WLAN credentials =====

#include "secrets.h"

// need a WebServer

ESP8266WebServer server(80);

// DisplayAdapterSSD1306 *display = NULL; // created later, when available
DisplayAdapter *display = NULL; // created later, when available

// ===== application state variables =====

char devicename[32]; // name of this device on the network

Board mainBoard;

// ===== implement =====

// Send out a JSON object with all files in dir
void handleFileList()
{
  DEBUG_LOG("handleFileList\n");

  String json;
  json.reserve(512);

  json = "[";
  Dir dir = SPIFFS.openDir("/");

  while (dir.next()) {
    json += "{'type': 'file',";
    json += "'name':'" + dir.fileName() + "',";
    json += "'size':" + String(dir.fileSize());
    json += "},\n";
  } // while

  json += "]";
  json.replace('\'', '"');
  json.replace(",\n]", "\n]");

  server.send(200, "text/json", json);
} // handleFileList


/**
 * Setup all components and Serial debugging helpers
 */
void setup(void)
{
  unsigned long now = millis();

  Serial.begin(115200);
  Serial.setDebugOutput(false);

  WiFi.mode(WIFI_OFF);

  // Logger::logger_level = LOGGER_LEVEL_TRACE;

  LOGGER_INFO("HomDing Device is starting... (%s), %d", __DATE__,
              ESP.getBootMode());

  // ----- setup File System -----
  SPIFFS.begin();

  // ----- load configuration -----

  mainBoard.init(&server);

  // load all config files and create Elements
  mainBoard.addElements();

  // ----- setup Display -----

  // TODO: will be done by the configured Display Element.

  // for Esp-WRoom-02 Module ESP8266 with OLED and 18650
#if 0
  display = (DisplayAdapter *)(new DisplayAdapterSSD1306(0x3c, 5, 4, 64));
#endif

#if 1
  display = (DisplayAdapter *)(new DisplayAdapterLCD(0x27, SDA, SCL));
#endif

  // for Wifi Kit 8 Module ESP8266 with OLED
#if 0
  // the propper reset of the OLED is required using pin 16 = D0
  pinMode(D0, OUTPUT);
  digitalWrite(D0, LOW); // turn D2 low to reset OLED
  delay(50);
  digitalWrite(D0, HIGH); // while OLED is running, must set D2 in high

  // for Wifi-Kit 8
  display = new DisplayAdapterSSD1306(0x3c, 4, 5, 32);
#endif

  mainBoard.display = display;

  if (display) {
    if (display->init()) {
      display->drawText(0, 0, 0, "HomeDing...");
      delay(100);

    } else {
      DEBUG_LOG("no attached display found.\n");
      delete display;
      display = NULL;
    } // if
  } // if

  // ----- setup Board and Elements-----

  Element *deviceElement = mainBoard.getElement("device");

  if (deviceElement) {
    strncpy(devicename, deviceElement->get("name"), sizeof(devicename));
    DEBUG_LOG(" devicename: %s.\n", devicename);
  } else {
    strncpy(devicename, "homeding", sizeof(devicename));
  } // if

  // ----- setup Server -----

  WiFi.mode(WIFI_STA);
  if ((*ssid) && (*password)) {
    WiFi.begin(ssid, password);
  } else {
    WiFi.begin();
  }

  // set device hostname as soon as possible from the device name
  WiFi.hostname(devicename);
  WiFi.setAutoConnect(true);

  // ===== start Elements =====
  mainBoard.start();
  DEBUG_LOG("Elements started.\n\n");


  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\n");

  IPAddress ipAddress = WiFi.localIP();
  char ipstr[16];
  sprintf(ipstr, "%u.%u.%u.%u", ipAddress[0], ipAddress[1], ipAddress[2], ipAddress[3]);

  DEBUG_LOG("\nConnected to: %s\n", WiFi.SSID().c_str());
  DEBUG_LOG("  as: %s\n", devicename);
  DEBUG_LOG("  IP: %s\n", ipstr);

  if (display) {
    display->clear();
    display->drawText(0, 0, 0, devicename);
    delay(800);
    display->clear();
    display->drawText(0, 0, 0, ipstr);
    delay(800);
    display->clear();
  }

  // WiFi.printDiag(Serial);

  // ----- adding web server handlers -----

  // redirect to index.htm of only domain name is given.
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Location", "/index.htm", true);
    DEBUG_LOG("Redirect...");
    server.send(301, "text/plain", "");
  });

  // get heap status, analog input value and all GPIO statuses in one json
  // call
  server.on("/$sysinfo", HTTP_GET, []() {
    FSInfo fs_info;

    String json = "{";
    json += " 'build': '" __DATE__ "'\n";
    json += " 'free heap':" + String(ESP.getFreeHeap()) + ",\n";

    json += " 'flash-size':" + String(ESP.getFlashChipSize()) + ",\n";
    json += " 'flash-real-size':" + String(ESP.getFlashChipRealSize()) + ",\n";

    SPIFFS.info(fs_info);
    json += " 'fs-totalBytes':" + String(fs_info.totalBytes) + ",\n";
    json += " 'fs-usedBytes':" + String(fs_info.usedBytes) + "\n";


    json += "}";
    json.replace('\'', '"');

    server.send(200, "text/json", json);
    json = String();
  });

  // list directory
  server.on("/$list", HTTP_GET, handleFileList);

  // Data and Commands exchange
  // https://gist.github.com/igrr/0da0c4adc7588d9bd911
  // https://www.intertech.com/Blog/iot-with-an-esp8266-part-2-arduino-sketch/
  // https://learn.adafruit.com/esp8266-temperature-slash-humidity-webserver/code

  server.addHandler(new BoardHandler("/$board", mainBoard));

  server.on("/$reboot", HTTP_GET, []() {
    DEBUG_LOG("rebooting...\n");
    server.send(200, "text/plain", "");
    delay(500);
    ESP.reset();
  });

  server.addHandler(new FileServerHandler(SPIFFS, "/", "NO-CACHE"));
  server.begin();

  DEBUG_LOG("Server started.\n\n");
} // setup


// handle all give time to all Elements and active components.
void loop(void)
{
  server.handleClient();
  mainBoard.loop();
} // loop()


// end.
