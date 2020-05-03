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
#include <Element.h>
#include <Board.h>

#include <ElementRegistry.h>
#include <time/NTPTimeElement.h>

#include <sntp.h>
#include <time.h>

// String constants, only once in Memory
static const char *NTPE_ntpserver = "ntpserver";

static const char *NTPE_zone = "zone";
static const char *NTPE_readtime = "readtime";

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
  _zone = 1; // Central Europe
  _nextRead = 0;
  _readTime = 24 * 60 * 60; // once a day
} // NTPTimeElement()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool NTPTimeElement::set(const char *name, const char *value)
{
  bool ret = true;

  if (_stricmp(name, NTPE_ntpserver) == 0) {
    _ntpServer = value;

  } else if (_stricmp(name, NTPE_readtime) == 0) {
    _readTime = _atotime(value);

  } else if (_stricmp(name, NTPE_zone) == 0) {
    _zone = _atoi(value);

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

  _nextRead = now + 4; // now + min. 2 sec., don't hurry
  _sendTime = 0;
  _state = 0;
  Element::start();

  sntp_stop();
  sntp_set_timezone(_zone);
  // sntp_set_daylight(3600);
  sntp_setservername(0, (char *)_ntpServer.c_str());
} // start()


/**
 * @brief check the state of the local time DHT and eventually request a new ntp
 * time.
 */
void NTPTimeElement::loop()
{
  unsigned int now = _board->getSeconds();

  if ((_state == 0) && (_nextRead < now)) {
    LOGGER_ETRACE("request sync");
    sntp_init();
    _state = 1;
    _sendTime = now;

  } else if (_state == 1) {
    if (now - _sendTime < 4) {
      // wait for response
      uint32 current_stamp = sntp_get_current_timestamp();
      if (current_stamp < (24 * 60 * 60)) {
        // not synced yet => wait.
      } else {
        LOGGER_EINFO("got: %d", current_stamp);
        _state = 0;
        _nextRead = now + _readTime;
        sntp_stop();

      } // if

    } else {
      // no response within 4 seconds.
      LOGGER_EERR("no response");
      _state = 0;
      _nextRead = now + 4; // try in some seconds again
      sntp_stop();
    } // if
  } // if
} // loop()


void NTPTimeElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  time_t tStamp = sntp_get_current_timestamp();
  char tmp[32];

  Element::pushState(callback);
  strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&tStamp));
  callback("now", tmp);
} // pushState()


// End.
