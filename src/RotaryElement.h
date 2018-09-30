/**
 * @file RotaryElement.h
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
 * * 15.09.2018 created by Matthias Hertel
 */

#ifndef RotaryElement_H
#define RotaryElement_H

#include "Board.h"
#include "Element.h"
#include <Arduino.h>

#include <RotaryEncoder.h>

/**
 * @brief RotaryElement implements...
 * @details
@verbatim

The RotaryElement can ...

@endverbatim
 */

class RotaryElement : public Element
{
public:
  /**
   * @brief Factory function to create a RotaryElement.
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
   * @brief The actual value.
   */
  long _value = 0;

  /**
   * @brief delta of a value change
   */
  int _step = 1;

  /**
   * @brief Pin #1 for rotary input
   */
  int _pin1 = -1;

  /**
   * @brief Pin #2 for rotary input
   */
  int _pin2 = -1;

  /**
   * @brief The _changeAction holds the actions that are submitted when the
   * rotary was changed.
   */
  String _changeAction;
};


#ifdef HOMEDING_REGISTER
// Register the RotaryElement onto the ElementRegistry.
bool RotaryElement::registered =
    ElementRegistry::registerElement("rotary", RotaryElement::create);
#endif

#endif