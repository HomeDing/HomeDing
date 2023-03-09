/**
 * @file PWMOutElement.h
 * @brief Output Element of the HomeDing Library to output an digital signals based on actions.
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
 */

#pragma once

#include <HomeDing.h>

/**
 * @brief The PWMOutElement is an Element that creates an pwm digital output signal based on actions.
 */
class PWMOutElement : public Element
{
public:
  /**
   * @brief Factory function to create a PWMOutElement.
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
   * @brief Activate the PWMOutElement.
   * @return true when activation was good.
   * @return false when activation failed.
   */
  virtual void start() override;

  virtual void pushState(
      std::function<void(const char *pName, const char *eValue)> callback) override;

private:
  int _pin = -1;

#if defined(ESP32)
  /** channel for ESP32 */
  int _channel;
#endif

  // when _inverse == true: logical HIGH will produce a physical LOW level.
  bool _inverse = false;

  // Set the range from 0 to _range. 0...1023 is the default. 
  int _range = 1023;

  int _value = 0;

  void _setValue(int newValue); // set the new value.
};


#ifdef HOMEDING_REGISTER
// Register the PWMOutElement onto the ElementRegistry.
bool PWMOutElement::registered =
    ElementRegistry::registerElement("pwmout", PWMOutElement::create);
#endif
