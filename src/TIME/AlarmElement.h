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

#ifndef AlarmELEMENT_H
#define AlarmELEMENT_H

#include <Arduino.h>
#include <Element.h>

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
   * @brief initialize a new Element.
   * @param board The board reference.
   */
  // virtual void init(Board *board);

  /**
   * @brief Set a parameter or property to a new value or start an action.
   * @param name Name of property.
   * @param value Value of property.
   * @return true when property could be changed and the corresponding action
   * could be executed.
   */
  virtual bool set(const char *name, const char *value);

  /**
   * @brief Activate the Element.
   * @return true when the Element could be activated.
   * @return false when parameters are not usable.
   */
  virtual void start();

  /**
   * @brief Give some processing time to the timer to check for next action.
   */
  virtual void loop();

  /**
   * @brief push the current value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void pushState(
      std::function<void(const char *pName, const char *eValue)> callback);

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

#endif