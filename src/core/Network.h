
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

#define NETWORKTRACE(...) LOGGER_JUSTINFO("Net:" __VA_ARGS__)

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

  static void init(){
    // state = NETSTATE::NONET;
  };

  static void connect(String &deviceName, String &nName, String &nPass) {
    Logger::printf("connect as '%s' to WiFI %s...", deviceName.c_str(), nName.c_str());

#if defined(ESP32)
    // https://stackoverflow.com/questions/54907985/esp32-fails-on-set-wifi-hostname
    WiFi.disconnect(true);
    WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
#endif

    WiFi.hostname(deviceName);

    // Evil bad hack to get the hostname set up correctly
#if defined(ESP32)
    WiFi.mode(WIFI_AP_STA);
#endif
    WiFi.mode(WIFI_STA);
    WiFi.begin(nName.c_str(), nPass.c_str());
    delay(100);
    state = NETSTATE::CONNECTSTA;
  };

  static void loop() {

    wl_status_t thisState = WiFi.status();
    static wl_status_t _wifi_status = WL_NO_SHIELD;

    if (thisState != _wifi_status) {

      if ((state == NETSTATE::CONNECTSTA) && (thisState == WL_CONNECTED)) {
        state = NETSTATE::CONNECTED;
        WiFi.setAutoReconnect(true);

      } else if ((state == NETSTATE::CONNECTED) && (thisState == WL_CONNECTION_LOST)) {
        state = NETSTATE::RECONNECT;

      } else if ((state == NETSTATE::CONNECTSTA) && ((thisState == WL_NO_SSID_AVAIL) || (thisState == WL_CONNECT_FAILED))) {
        state = NETSTATE::FAILED;
      }

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

      _wifi_status = thisState;
    }
  };
};