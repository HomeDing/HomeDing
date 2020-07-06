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
#include <Board.h>
#include <Element.h>

#include <ElementRegistry.h>
#include <time/NTPTimeElement.h>

#include <TZ.h>
#include <coredecls.h> // settimeofday_cb()
#include <sntp.h>
#include <sys/time.h>
#include <time.h>

// String constants, only once in Memory
static const char *NTPE_ntpserver = "ntpserver";
static const char *NTPE_zone = "zone";


/**
 * @brief static factory function to create a new NTPTimeElement
 * @return NTPTimeElement* created element
 */
Element *NTPTimeElement::create()
{
  return (new NTPTimeElement());
} // create()


NTPTimeElement::NTPTimeElement()
{
  // set some defaults
  _ntpServer = "pool.ntp.org";
  _timezone = TZ_Europe_Berlin; // e.g. "CET-1CEST,M3.5.0,M10.5.0/3"
} // NTPTimeElement()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool NTPTimeElement::set(const char *name, const char *value)
{
  bool ret = true;

  if (_stricmp(name, NTPE_ntpserver) == 0) {
    _ntpServer = value;

  } else if (_stricmp(name, NTPE_zone) == 0) {
    _timezone = value;

  } else {
    ret = Element::set(name, value);
  } // if
  return (ret);
} // set()


/**
 * @brief Activate the NTPTimeElement.
 */
void NTPTimeElement::start()
{
  unsigned long now = _board->getSeconds();

  Element::start();
  configTime(_timezone.c_str(), _ntpServer.c_str());
} // start()

void NTPTimeElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  time_t tStamp = time(nullptr);
  char tmp[32];

  Element::pushState(callback);
  strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&tStamp));
  callback("now", tmp);
} // pushState()


// End.
