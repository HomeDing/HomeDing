/**
 * @file AnalogElement.h
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
 * * 29.11.2018 created by Matthias Hertel
 * * 17.06.2022 refactored to a SensorElement
 */

#pragma once

#include <sensors/SensorElement.h>

/**
 * @brief The AnalogElement is an special Element that creates actions based on
 * a digital IO signal.
 */
class AnalogElement : public SensorElement {
public:
  /**
   * @brief Factory function to create a AnalogElement.
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

  // Sensor Element functions

  /// retrieve values from a sensor
  bool getProbe(UNUSED String &values) override;

  /// send data out by crating actions
  void sendData(UNUSED String &values) override;

private:
#if defined ESP8266
  int _pin = A0;  // there is only one pin on ESP8266
#else
  int _pin = -1;  // pin must be configured
#endif

  // map() function just like the standard Arduino map() function but with float and boundaries from configuration.
  int map(int value);

  // map factors
  bool _useMap = false;  // use map function when all 4 factors are given.
  int _inMin = 0, _inMax = 0, _outMin = 0, _outMax = 0;
  bool _constrain = false;

  int _hysteresis = 10;
  int _reference = 500;

  int _lastValue;
  int _lastReference;

  /** These actions are sent when the current value is above the reference value. */
  String _highAction;

  /** These actions are sent when the current value is below or equal the reference value. */
  String _lowAction;
};

#ifdef HOMEDING_REGISTER
// Register the AnalogElement onto the ElementRegistry.
bool AnalogElement::registered = ElementRegistry::registerElement("analog", AnalogElement::create);
#endif
