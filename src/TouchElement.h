/**
 * @file TouchElement.h
 *
 * @brief Input Element for the HomeDing Library enabling the Touch functionality
 * by the ESP32 processors.
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
 * * 20.06.2022 created by Matthias Hertel
 */

#pragma once

#include <HomeDing.h>

/**
 * @brief The TouchElement is an special Element that creates actions based
 * on a digital IO signal.
 */
class TouchElement : public Element {
public:
  /**
   * @brief Factory function to create a TouchElement.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;

#if defined(ESP32)
  /**
   * @brief Set a parameter or property to a new value or start an action.
   * @param name Name of property.
   * @param value Value of property.
   * @return true when property could be changed and the corresponding action
   * could be executed.
   */
  virtual bool set(const char *name, const char *value) override;

  /** @brief Activate the Element.*/
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
#endif

private:
  /**
   * @brief GPIO pin to be used for physical signal.
   */
  int _pin = -1;

  int _reference = 65;

  int _value;

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

#if defined(HOMEDING_REGISTER)

#if defined(ESP32)
// Register the TouchElement onto the ElementRegistry.
bool TouchElement::registered =
  ElementRegistry::registerElement("touch", TouchElement::create);

#elif defined(ESP8266)
#warning Touch Element Input is not supported on ESP8266 processors  
#endif

#endif
