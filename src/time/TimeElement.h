/**
 * @file TimeElement.h
 * 
 * @brief Core Element for the HomeDing Library to send actions based on the actual local time.
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
 * * 09.10.2018 created by Matthias Hertel
 */

#ifndef TIMEELEMENT_H
#define TIMEELEMENT_H

#include <Arduino.h>
#include <Element.h>

/**
 * @brief The TimeElement is an special Element that creates actions based on
 * a digital IO signal.
 */
class TimeElement : public Element
{
public:
  /**
   * @brief Factory function to create a TimeElement.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;

  /**
   * @brief Construct a new TimeElement
   */
  TimeElement();

  /**
   * @brief Set a parameter or property to a new value or start an action.
   * @param name Name of property.
   * @param value Value of property.
   * @return true when property could be changed and the corresponding action
   * could be executed.
   */
  virtual bool set(const char *name, const char *value);

  /**
   * @brief Activate the TimeElement.
   * @return true when activation was good.
   * @return false when activation failed.
   */
  virtual void start();

  /**
   * @brief check the state of the DHT values and eventually create actions.
   */
  virtual void loop();

private:
  void _sendAction(String &action, const char *fmt, time_t tStamp);

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

#endif // TIMEELEMENT_H