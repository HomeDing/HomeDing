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

#pragma once

#include <HomeDing.h>

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
  virtual bool set(const char *name, const char *value) override;

  /**
   * @brief Activate the TimeElement.
   * @return true when activation was good.
   * @return false when activation failed.
   */
  virtual void start() override;

  /**
   * @brief check the state of the DHT values and eventually create actions.
   */
  virtual void loop() override;

private:
  /**
   * @brief Send out the action after adding the formatted value.
   * @param action Action template.
   * @param fmt Format for the value.
   * @param tmp the local time
   */
  void _sendAction(String &action, const char *fmt, struct tm *tmp);

  time_t _lastTimestamp;
  time_t _lastMinute;
  time_t _lastDate;

  // action send everytime the time has changed, value = hh:mm:ss
  String _timeAction;

  // action send everytime the date has changed, value = YYYY-MM-DD hh:mm:ss
  String _timestampAction;

  // action send everytime the time has changed, value = hh:mm
  String _minuteAction;

  // action send everytime the date has changed, value = YYYY-MM-DD
  String _dateAction;
};

// ===== Register =====

#ifdef HOMEDING_REGISTER
// Register the TimeElement onto the ElementRegistry.
bool TimeElement::registered =
    ElementRegistry::registerElement("time", TimeElement::create);
#endif
