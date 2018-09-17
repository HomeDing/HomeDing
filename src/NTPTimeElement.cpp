/**
 * @file NTPTimeElement.cpp
 *
 * @brief Core Element for the HomeDing Library to get the actual time using the
 * NTP protocol.
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
 * The sntp functions are documented in the ESP8266 Non-OS SDK  API Reference
 * See
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
 * Changelog: see NTPTimeElement.h
 */

#include "NTPTimeElement.h"
#include "ElementRegistry.h"

#define LOGGER_MODULE "NTPTime"
#include "core/Logger.h"

#include "sntp.h"
#include "time.h"

#include "user_interface.h"

// String constants, only once in Memory
static const char *NTPE_ntpserver = "ntpserver";

static const char *NTPE_zone = "zone";
static const char *NTPE_readtime = "readtime";

static const char *NTPE_ontime = "ontime";
static const char *NTPE_onminute = "onminute";
static const char *NTPE_ondate = "ondate";
static const char *NTPE_ontimestamp = "ontimestamp";

static const char *NTPE_timeFmt = "%H:%M:%S";
static const char *NTPE_minuteFmt = "%H:%M";
static const char *NTPE_dateFmt = "%Y-%m-%d";
static const char *NTPE_timestampFmt = "%Y-%m-%d %H:%M:%S";

/**
 * @brief static factory function to create a new NTPTimeElement
 * @return NTPTimeElement* created element
 */
Element *NTPTimeElement::create()
{
  LOGGER_TRACE("create()");
  return (new NTPTimeElement());
} // create()


NTPTimeElement::NTPTimeElement()
{
  // set some defaults
  _ntpServer = "pool.ntp.org";
  _zone = 1; // Central Europe
  _nextRead = 0;

  uint32 rtc_time = system_get_rtc_time();
  LOGGER_INFO("rtc_time=%d", rtc_time);
} // NTPTimeElement()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool NTPTimeElement::set(const char *name, const char *value)
{
  LOGGER_TRACE("set(%s:%s)", name, value);
  bool ret = true;

  if (_stricmp(name, NTPE_ntpserver) == 0) {
    _ntpServer = value;

  } else if (_stricmp(name, NTPE_readtime) == 0) {
    _readTime = _atotime(value);

  } else if (_stricmp(name, NTPE_zone) == 0) {
    _zone = atoi(value);

  } else if (_stricmp(name, NTPE_ontime) == 0) {
    _timeAction = value;

  } else if (_stricmp(name, NTPE_onminute) == 0) {
    _minuteAction = value;

  } else if (_stricmp(name, NTPE_ondate) == 0) {
    _dateAction = value;

  } else if (_stricmp(name, NTPE_ontimestamp) == 0) {
    _timestampAction = value;

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
  LOGGER_TRACE("start()");
  unsigned long now = (millis() / 1000);

  _nextRead = now + 2; // now + min. 2 sec., don't hurry
  _sendTime = 0;
  _state = 0;
  Element::start();

  sntp_set_timezone(_zone);

  // time_t tStamp = (time_t)sntp_get_current_timestamp();
  // char b[32];
  // strftime(b, sizeof(b), NTPE_timestampFmt, localtime(&tStamp));
  // LOGGER_INFO("startup Time= %s", b);
} // start()


/**
 * @brief check the state of the DHT values and eventually create actions.
 */
void NTPTimeElement::loop()
{
  unsigned long now_m = millis();
  unsigned int now = now_m / 1000;
  time_t cts;
  uint32 current_stamp = sntp_get_current_timestamp();
  String out;
  String tmpEvent;

  time(&cts);

  if ((_state != 1) && (_nextRead < now)) {
    LOGGER_TRACE("request sntp sync");
    sntp_setservername(0, (char *)_ntpServer.c_str());
    sntp_set_timezone(_zone);
    // sntp_set_daylight(3600);

    sntp_init();
    _state = 1;
    _sendTime = now_m;

  } else if ((_state == 1) && (now_m - _sendTime < 8000)) {
    if (current_stamp < (24 * 60 * 60)) {
      // not synced yet => wait.
    } else {
      LOGGER_LOG("got time: %d", current_stamp);
      _state = 2;
      _nextRead = now + _readTime;
      _lastTimestamp = 0;
      sntp_stop();

    } // if

  } else if (_state == 1) {
    // no respnse within 8 seconds.
    LOGGER_ERR("No NTP Response :-(");
    _state = 0;
    _nextRead = now + _readTime;
    sntp_stop();

  } else if (_state == 2) {

    // check for time actions...
    if (_lastTimestamp != current_stamp) {
      _sendAction(_timeAction, NTPE_timeFmt, (time_t)current_stamp);
      _sendAction(_timestampAction, NTPE_timestampFmt, (time_t)current_stamp);
      _lastTimestamp = current_stamp;
    } // if

    // ignore seconds
    current_stamp -= (current_stamp % 60);
    if (_lastMinute != current_stamp) {
      _sendAction(_minuteAction, NTPE_minuteFmt, (time_t)current_stamp);
      _lastMinute = current_stamp;
    } // if

    // ignore time
    current_stamp -= (current_stamp % (24 * 60 * 60));
    if (_lastDate != current_stamp) {
      _sendAction(_dateAction, NTPE_dateFmt, (time_t)current_stamp);
      _lastDate = current_stamp;
    } // if
  } // if
} // loop()


void NTPTimeElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  time_t tStamp = sntp_get_current_timestamp();
  char tmp[32];

  Element::pushState(callback);
  strftime(tmp, sizeof(tmp), NTPE_timestampFmt, localtime(&tStamp));
  callback("now", tmp);
} // pushState()


// ===== private functions =====

/**
 * @brief Send out the action after adding the formatted value.
 * @param action Action template.
 * @param fmt Format for the value.
 * @param tStamp the time
 */
void NTPTimeElement::_sendAction(String &action, const char *fmt, time_t tStamp)
{
  if (action.length()) {
    LOGGER_TRACE("_send(%s)", action.c_str());
    char b[32];
    struct tm *tmp = localtime(&tStamp);
    strftime(b, sizeof(b), fmt, tmp);
    _board->dispatch(action, b);
  } // if
} // _sendAction()


// ===== Register =====

// Register the NTPTimeElement onto the ElementRegistry.
bool NTPTimeElement::registered =
    ElementRegistry::registerElement("ntptime", NTPTimeElement::create);

// End.
