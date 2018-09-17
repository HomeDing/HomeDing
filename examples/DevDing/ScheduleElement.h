/**
 * @file ScheduleElement.h
 * @brief Element Template class.
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

#include "Board.h"
#include "Element.h"
#include <Arduino.h>

/**
 * @brief The ScheduleElement can trigger a ON and OFF action per day based on
 * real time.
 */
class ScheduleElement : public Element
{
public:
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
   * @brief initialize a new Element.
   * @param board The board reference.
   */
  virtual void init(Board *board);

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
   * @brief The actual value.
   */
  int _value;

  /**
   * @brief remember that a first time was processed already.
   */
  bool _init;

  time_t _onTime;
  time_t _offTime;

  /**
   * @brief The _xAction holds the actions that is submitted when ...
   */
  String _onAction;
  String _offAction;
};

#endif