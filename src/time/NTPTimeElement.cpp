/**
 * @file NTPTimeElement.cpp
 *
 * @brief Core Element for the HomeDing Library to get the actual local time
 * using the NTP protocol from a NTP server.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license,
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino.
 *
 * The sntp functions are documented in the ESP8266 Non-OS SDK API Reference
 * https://www.espressif.com/sites/default/files/documentation/2c-esp8266_non_os_sdk_api_reference_en.pdf
 *
 * For format options in strftime see
 * http://en.cppreference.com/w/cpp/chrono/c/strftime
 *
 * rtc and sntp sync on startup:
 * https://github.com/nodemcu/nodemcu-firmware/blob/master/app/include/rtc/rtctime_internal.h
 * system_rtc_mem_read
 * https://github.com/Makuna/Task
 *
 * Changelog: see time/NTPTimeElement.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <time/NTPTimeElement.h>

#if defined(ESP8266)
#include <TZ.h>

#elif defined(ESP32)
// There is no TZ definition in ESP32 SDK so include LONDON timezone here as an example.
#define TZ_Europe_London PSTR("GMT0BST,M3.5.0/1,M10.5.0")

#include "time.h"

#endif


/**
 * @brief static factory function to create a new NTPTimeElement
 * @return NTPTimeElement* created element
 */
Element *NTPTimeElement::create() {
  return (new NTPTimeElement());
} // create()


NTPTimeElement::NTPTimeElement() {
  // set some defaults
  category = CATEGORY::Standard; // no polling
  _ntpServer = "pool.ntp.org";
  _timezone = TZ_Europe_London; // e.g. "CET-1CEST,M3.5.0,M10.5.0/3"
} // NTPTimeElement()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool NTPTimeElement::set(const char *name, const char *value) {
  bool ret = true;

  if (_stricmp(name, "ntpserver") == 0) {
    _ntpServer = value;

  } else if (_stricmp(name, "zone") == 0) {
    _timezone = value;

  } else {
    ret = Element::set(name, value);
  } // if
  return (ret);
} // set()


/**
 * @brief Activate the NTPTimeElement.
 */
void NTPTimeElement::start() {
  Element::start();
  configTzTime(_timezone.c_str(), _ntpServer.c_str());
} // start()

void NTPTimeElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback) {
  time_t tStamp = time(nullptr);
  char tmp[32];

  Element::pushState(callback);
  strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&tStamp));
  callback("now", tmp);
} // pushState()


// End.
