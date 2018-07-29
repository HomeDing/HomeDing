/**
 * @file NTPTimeElement.h
 * 
 * @brief Core Element for the HomeDing Library to get the actual time using the NTP protocol.
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
 * Changelog:
 * * 21.05.2018 created by Matthias Hertel
 */

#ifndef NTPTIMEELEMENT_H
#define NTPTIMEELEMENT_H

#include <Arduino.h>
#include "Board.h"
#include "Element.h"

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

// not properties:

// local port to listen for UDP packets
#define localPort 1337


/**
 * @brief The NTPTimeElement is an special Element that creates actions based on
 * a digital IO signal.
 */
class NTPTimeElement : public Element
{
public:
  /**
   * @brief Factory function to create a NTPTimeElement.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;

  /**
   * @brief Construct a new NTPTimeElement object.
   */
  NTPTimeElement();

  /**
   * @brief Set a parameter or property to a new value or start an action.
   * @param name Name of property.
   * @param value Value of property.
   * @return true when property could be changed and the corresponding action
   * could be executed.
   */
  virtual bool set(const char *name, const char *value);

  /**
   * @brief Activate the NTPTimeElement.
   * @return true when activation was good.
   * @return false when activation failed.
   */
  virtual void start();

  /**
   * @brief check the state of the DHT values and eventually create actions.
   */
  virtual void loop();

  /**
   * @brief push the current value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void
  pushState(std::function<void(const char *pName, const char *eValue)> callback);

private:
  // host name of ntp server
  String _ntpServer;

  // Time Zone
  int _zone;

  // time between 2 time syncronizations
  int _readTime;

  // 0: no time: 1 receiving time, 2: time ok.
  int _state;

 // millis, when a packet was sent out. 0 means no packet was sent.
  unsigned long _sendTime;

  void _sendAction(String &action, const char *fmt, time_t tStamp);

  unsigned long _nextRead;
  uint32 _lastTimestamp;
  uint32 _lastMinute;
  uint32 _lastDate;

  // action send everytime the time has changed, value = hh:mm:ss
  String _timeAction;

  // action send everytime the date has changed, value = YYYY-MM-DD hh:mm:ss
  String _timestampAction;

  // action send everytime the time has changed, value = hh:mm
  String _minuteAction;

  // action send everytime the date has changed, value = YYYY-MM-DD
  String _dateAction;


};

#endif // NTPTIMEELEMENT_H