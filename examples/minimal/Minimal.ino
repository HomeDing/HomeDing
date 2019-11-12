/**
 * @file Minimal.ino
 * @brief Minimal Sketch that uses the HomeDing Library to implement Things attached the
 * Internet.
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license,
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino and https://homeding.github.io/.
 *
 * Changelog:
 * * 12.11.2019 Minimal Example created from development sketch.
 */

#include <Arduino.h>
#include <Board.h>
#include <Element.h>

#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#include <FS.h>

#include <BoardServer.h>
#include <ElementRegistry.h>
#include <FileServer.h>

#define HOMEDING_REGISTER 1

// Use the Core Elements of the HomeDing Library
// #define HOMEDING_INCLUDE_CORE

#define HOMEDING_INCLUDE_Value
#define HOMEDING_INCLUDE_Button
#define HOMEDING_INCLUDE_Switch
#define HOMEDING_INCLUDE_AND
#define HOMEDING_INCLUDE_Analog
#define HOMEDING_INCLUDE_Timer
#define HOMEDING_INCLUDE_Schedule
#define HOMEDING_INCLUDE_Alarm
#define HOMEDING_INCLUDE_DigitalIn
#define HOMEDING_INCLUDE_DigitalOut
#define HOMEDING_INCLUDE_PWMOut
// #define HOMEDING_INCLUDE_LOG
#define HOMEDING_INCLUDE_PMS

// Use some more Elements that need additional libraries
#define HOMEDING_INCLUDE_DHT
#define HOMEDING_INCLUDE_BME680
#define HOMEDING_INCLUDE_DS18B20
// #define HOMEDING_INCLUDE_RFSend
// #define HOMEDING_INCLUDE_ROTARY
// #define HOMEDING_INCLUDE_MENU

#define HOMEDING_INCLUDE_NTPTIME
// #define HOMEDING_INCLUDE_DSTIME

// Use some more Elements for Displays
// #define HOMEDING_INCLUDE_DISPLAY
// #define HOMEDING_INCLUDE_DISPLAYLCD
// #define HOMEDING_INCLUDE_DISPLAYSSD1306
// #define HOMEDING_INCLUDE_DISPLAYSH1106

// #define HOMEDING_INCLUDE_SERIALLINE

#include <HomeDing.h>
#include <MicroJsonComposer.h>

static const char respond404[] PROGMEM =
R"==(<html><head><title>File not found</title></head><body>
File not found</body></html>)==";

// ===== WLAN credentials =====

#include "secrets.h"

// need a WebServer
ESP8266WebServer server(80);

// ===== application state variables =====

Board mainBoard;

// ===== implement =====

void handleRedirect()
{
  LOGGER_RAW("Redirect...");

  String url;
  if (mainBoard.boardState < BOARDSTATE_STARTCAPTIVE) {
    url = mainBoard.homepage;
  } else {
    url = "http://";
    url.concat(WiFi.softAPIP().toString()); // mainBoard.deviceName
    url.concat("/$setup");
  }
  server.sendHeader("Location", url, true);
  server.send(302);
  server.client().stop();
} // handleRedirect()


/**
 * Setup all components and Serial debugging helpers
 */
void setup(void)
{
  Serial.begin(115200);

  Serial.setDebugOutput(false);

  LOGGER_INFO("HomeDing Device is starting...");

  // ----- setup the file system and load configuration -----
  SPIFFS.begin();
  mainBoard.init(&server);

  // ----- adding web server handlers -----
  // redirect to index.htm when only domain name is given.
  server.on("/", HTTP_GET, handleRedirect);

  // Board status and actions
  server.addHandler(new BoardHandler(&mainBoard));

  // Static files in the file system.
  server.addHandler(new FileServerHandler(SPIFFS, "NO-CACHE"));

  server.onNotFound([]() {
    const char *uri = server.uri().c_str();
    LOGGER_RAW("notFound: %s", uri);

    if ((mainBoard.boardState == BOARDSTATE_RUNCAPTIVE) &&
        ((strcmp(uri, "/connecttest.txt") == 0) ||
         (strcmp(uri, "/redirect") == 0) ||
         (strcmp(uri, "/generate_204") == 0) ||
         (strcmp(uri, "/more.txt") == 0))) {
      handleRedirect();
    } else {
      // standard not found in browser.
      server.send(404, TEXT_HTML, FPSTR(respond404));
    }
  });

  LOGGER_INFO("setup done.");
} // setup


// handle all give time to all Elements and active components.
void loop(void)
{
  server.handleClient();
  mainBoard.loop();
} // loop()


// end.
