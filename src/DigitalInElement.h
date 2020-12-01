/**
 * @file DigitalInElement.h
 *
 * @brief Core Input Element for the HomeDing Library typically used with
 * momentary buttons or switches.
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
 * * 29.04.2018 created by Matthias Hertel
 * * 16.09.2018 pullup config added.
 * * 02.02.2018 refactored to digitalIn and switch.
 */

#ifndef DIGITALIN_H
#define DIGITALIN_H

#include <HomeDing.h>

#define BUTTON_TYPE_LEVEL 0x00
#define BUTTON_TYPE_TOGGLE 0x01

// class OneButton;

/**
 * @brief The DigitalInElement is an special Element that creates actions based
 * on a digital IO signal.
 */
class DigitalInElement : public Element
{
public:
  /**
   * @brief Factory function to create a DigitalInElement.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;

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
   * @return true when activation was good.
   * @return false when activation failed.
   */
  virtual void start() override;

  /**
   * @brief check the state of the input signal and eventually emit actions.
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
   * @brief GPIO pin to be used for physical signal.
   */
  int _pin = -1;

  /**
   * @brief Control level logic for input at _pin. When true: physical HIGH level will produce a logical LOW value.
   */
  bool _inverse = false;

  /**
   * @brief Control internal pullup. When true the internal pullup is activated.
   */
  bool _pullup = false;

  int _lastInLevel;

  /**
   * @brief The _highAction is emitted when the logical input level is going
   * from LOW to HIGH.
   */
  String _highAction;

  /**
   * @brief The _lowAction is emitted when the logical input level is going from
   * HIGH to LOW.
   */
  String _lowAction;

  /**
   * @brief The _valueAction is emitted every time the logical input level
   * changed.
   */
  String _valueAction;
};

#ifdef HOMEDING_REGISTER
// Register the DigitalInElement onto the ElementRegistry.
bool DigitalInElement::registered =
    ElementRegistry::registerElement("digitalin", DigitalInElement::create);
#endif

#endif // DIGITALIN_H