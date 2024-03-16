
/**
 * @file Network.h
 * @author Matthias Hertel (https://www.mathertel.de)
 *
 * @brief The Network class implements the connection of a device to different Access Points (networks).
 *
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 *
 * Changelog:
 * * 30.08.2023 creation
 */

#pragma once

#include <Arduino.h>
#include <HomeDing.h>

// #define NETWORKTRACE_ON

#if defined(NETWORKTRACE_ON)
#define NETWORKTRACE(...) LOGGER_JUSTINFO("Net:" __VA_ARGS__)
#else
#define NETWORKTRACE(...)
#endif

class Network {
public:
  // State of Network connectivity:
  //   NONET --(by board)-> CONNECTSTA --(by status)-> CONNECTED <-(by status)-> RECONNECT
  //   NONET --(by board)-> CREATEAP --(by status)-> CAPTIVE

  enum NETSTATE : int {
    // ===== startup operation states
    NONET = 0,        // no network connected
    CONNECTSTA = 11,  // start connecting to the network
    CONNECTED = 12,   // network is connected
    RECONNECT = 13,   // network is dropped, reconnect required.
    FAILED = 14,      // no network available to connect.

    CREATEAP = 21,  // Wait for network connectivity or configuration request.
    CAPTIVE = 22    // network is connected but wait for configuration request.
  };

  static NETSTATE state;  // current network state;

  static void init() {
    state = NETSTATE::NONET;
    WiFi.disconnect(true);
    WiFi.persistent(false);
  };

  static void connect(String &deviceName, String &nName, String &nPass) {
    Logger::printf("connect as '%s' to WiFI %s...", deviceName.c_str(), nName.c_str());

    WiFi.mode(WIFI_STA);

#if defined(ESP32)
    WiFi.setHostname(deviceName.c_str());
    delay(100);

#elif defined(ESP8266)
    WiFi.hostname(deviceName);
#endif

    WiFi.begin(nName.c_str(), nPass.c_str());
    delay(100);
    state = NETSTATE::CONNECTSTA;
  };


  static void loop() {
    static wl_status_t _wifi_status = WL_NO_SHIELD;

    wl_status_t thisState = WiFi.status();

    if (thisState != _wifi_status) {
      NETWORKTRACE("raw: %d", thisState);

      if ((state == NETSTATE::CONNECTSTA) && (thisState == wl_status_t::WL_CONNECTED)) {
        state = NETSTATE::CONNECTED;
        WiFi.setAutoReconnect(true);

      } else if ((state == NETSTATE::CONNECTED) && (thisState == WL_CONNECTION_LOST)) {
        state = NETSTATE::RECONNECT;

      } else if ((state == NETSTATE::CONNECTSTA) && ((thisState == WL_NO_SSID_AVAIL) || (thisState == WL_CONNECT_FAILED))) {
        state = NETSTATE::FAILED;
      }

#if defined(NETWORKTRACE_ON)
      const char *sText = "";
      switch (state) {
        case NETSTATE::NONET: sText = "No Net"; break;
        case NETSTATE::CONNECTSTA: sText = "connecting"; break;
        case NETSTATE::CONNECTED: sText = "connected"; break;
        case NETSTATE::RECONNECT: sText = "reconnect"; break;
        case NETSTATE::FAILED: sText = "failed"; break;
        case NETSTATE::CREATEAP: sText = "create Net"; break;
        case NETSTATE::CAPTIVE: sText = "captive"; break;
      }

      NETWORKTRACE("state: (%d) %s", thisState, sText);
#endif

      _wifi_status = thisState;
    }
  };
};