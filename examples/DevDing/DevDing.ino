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
#include <DisplayAdapterSSD1306.h>

// Library TabRF is used for sending RF 433 MHz Signals
#include <TabRF.h>
#include <intertechno2.h> // use the intertechno2 code defintions

// =====

#include <Board.h>
#include <BoardServer.h>
#include <FileServer.h>

#define LOGGER_MODULE "main"
#include <Logger.h>

// Use the Core Elements of the HomeDing Library
#define HOMEDING_INCLUDE_CORE

// Use some more Elements that need additional libraries
#define HOMEDING_INCLUDE_DHT
#define HOMEDING_INCLUDE_DS18B20
#define HOMEDING_INCLUDE_RFSend

#include <HomeDing.h>

extern "C" {
#include "user_interface.h"
}

// ===== WLAN credentials =====

// const char *ssid = "NetworkName";
// const char *password = "NetworkPass";
const char *ssid = "KH";
const char *password = "hk-021FD2829";

// need a WebServer

ESP8266WebServer server(80);

DisplayAdapterSSD1306 *display = NULL; // created later, when available

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

  //   Logger::logger_level = LOGGER_LEVEL_TRACE;

  LOGGER_INFO("Board Server is starting...");
  LOGGER_INFO("Build " __DATE__);
  LOGGER_INFO("Boot mode %d", ESP.getBootMode());

  // ----- setup File System -----
  SPIFFS.begin();

  // ----- setup Display -----

  // for Esp-Wroom-02 Modul ESP8266 with OLED and 18650 
#if 1
  display = new DisplayAdapterSSD1306(0x3c, 5, 4, 64);
#endif

  // for Wifi Kit 8 Modul ESP8266 with OLED
#if 0
  // the propper reset of the OLED is required using pin 16 = D0
  pinMode(D0, OUTPUT);
  digitalWrite(D0, LOW); // turn D2 low to reset OLED
  delay(50);
  digitalWrite(D0, HIGH); // while OLED is running, must set D2 in high

  // for Wifi-Kit 8
  display = new DisplayAdapterSSD1306(0x3c, 4, 5, 32);
#endif

  // ----- setup RF 433 MHz Library -----
#if 0
  // initialize the tabRF library
  tabRF.init(NO_PIN, D4); // input at D1 = pin #2 , output at D4, pin # 9
  tabRF.setupDefition(&Intertechno2_Sequence);
#endif

  if (!display->init()) {
    DEBUG_LOG("no attached display found.\n");
    delete display;
    display = NULL;
  } // if

  // ----- setup Board and Elements-----

  mainBoard.init((DisplayAdapter *)display, &server);
  mainBoard.addElements();
  mainBoard.start();
  DEBUG_LOG("Board started.\n\n");

  Element *deviceElement = mainBoard.getElement("device");

  if (deviceElement) {
    strncpy(devicename, deviceElement->get("name"), sizeof(devicename));
    DEBUG_LOG(" devicename: %s.\n", devicename);
    DEBUG_LOG(" description: %s.\n", deviceElement->get("description"));
  } else {
    strncpy(devicename, "homeding", sizeof(devicename));
  } // if


  // ----- setup Server -----

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  wifi_station_set_auto_connect(true);
  wifi_station_set_hostname(devicename);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\n");

  IPAddress ipno = WiFi.localIP();
  char ipstr[16];
  sprintf(ipstr, "%u.%u.%u.%u", ipno[0], ipno[1], ipno[2], ipno[3]);

  DEBUG_LOG("\nConnected to: %s\n", WiFi.SSID().c_str());
  DEBUG_LOG("  as: %s\n", devicename);
  DEBUG_LOG("  IP: %s\n", ipstr);

  if (display) {
    display->clear();
    display->drawText(0, 0, 10, devicename);
    display->drawText(0, 10, 10, ipstr);
    delay(4 * 1000);
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
    json += " 'heap':" + String(ESP.getFreeHeap()) + ",\n";

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