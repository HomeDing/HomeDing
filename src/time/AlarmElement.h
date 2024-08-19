/**
 * @file AlarmElement.h
 * @brief Create Action based on a specified time of the day.
 * 
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license.
 * https://www.mathertel.de/License.aspx.
 * 
 * More information on https://www.mathertel.de/Arduino
 * 
 * Changelog:
 * * 20.11.2018 created by Matthias Hertel
 */

#pragma once

#define TIME_T_NOT (48 * 60 * 60)

/**
 * @brief AlarmElement implements...
 * @details
@verbatim

The AlarmElement can ...
"alarm": {
  "wakeup": {
    "time": "08:00:00",
    "onTime": "",
    "description": "create Action at 8:00 in the morning." 
  }
}

@endverbatim
 */

class AlarmElement : public Element
{
public:
  /**
   * @brief Factory function to create a AlarmElement.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;

  /**
   * @brief Construct a new AlarmElement
   */
  AlarmElement();

  /**
   * @brief Set a parameter or property to a new value or start an action.
   * @param name Name of property.
   * @param value Value of property.
   * @return true when property could be changed and the corresponding action
   * could be executed.
   */
  virtual bool set(const char *name, const char *value) override;

  /**
   * @brief Activate the Element.
   */
  virtual void start() override;

  /**
   * @brief Give some processing time to the timer to check for next action.
   */
  virtual void loop() override;

private:
  /**
   * @brief The time the alarm event should be created.
   */
  time_t _time;


  time_t _lastTime = TIME_T_NOT;

  /**
   * @brief The _timeAction holds the actions that is submitted when ...
   */
  String _timeAction;
};

/* ===== Register the Element ===== */

#ifdef HOMEDING_REGISTER
// Register the AlarmElement onto the ElementRegistry.
bool AlarmElement::registered =
    ElementRegistry::registerElement("alarm", AlarmElement::create);
#endif
