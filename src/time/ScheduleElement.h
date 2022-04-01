/**
 * @file time/ScheduleElement.h
 * @brief The ScheduleElement creates actions based on the time of day.
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
 * * 30.07.2018 created by Matthias Hertel
 */

#ifndef ScheduleELEMENT_H
#define ScheduleELEMENT_H

#include <HomeDing.h>

/**
 * @brief The ScheduleElement can trigger a ON and OFF action per day based on
 * real time.
 */
class ScheduleElement : public Element
{
public:
  enum Mode {
    OFF = 0,  // outbound value is OFF(0).
    ON = 1,   // outbound value is ON(0).
    TIMER = 2 // outbound value by timer.
  };

  /**
   * @brief Factory function to create a ScheduleElement.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;

  /**
   * @brief Construct a new ScheduleElement
   */
  ScheduleElement();

  /**
   * @brief initialize a new Element.
   * @param board The board reference.
   */
  virtual void init(Board *board) override;

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

  /**
   * @brief push the current value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void pushState(
      std::function<void(const char *pName, const char *eValue)> callback) override;

private:
  /**
   * @brief Mode of operation.
   */
  Mode _mode = Mode::TIMER;

  /**
   * @brief The actual value.
   */
  bool _value = false;

  /**
   * @brief remember that a first time was processed already.
   */
  bool _init;

  time_t _startTime;
  time_t _endTime;

  /**
   * @brief The _onAction holds the actions that is submitted when the scheduled time period starts.
   */
  String _onAction;

  /**
   * @brief The _offAction holds the actions that is submitted when the scheduled time period ends.
   */
  String _offAction;

  /**
   * @brief The _valueAction holds the actions that is submitted when the scheduled time period starts or ends with a value of 1 and 0.
   */
  String _valueAction;
};

#ifdef HOMEDING_REGISTER
// Register the ScheduleElement onto the ElementRegistry.
bool ScheduleElement::registered =
    ElementRegistry::registerElement("schedule", ScheduleElement::create);
#endif

#endif