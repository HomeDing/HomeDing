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
 */

#ifndef ANALOG_H
#define ANALOG_H

#include <HomeDing.h>

/**
 * @brief The AnalogElement is an special Element that creates actions based on
 * a digital IO signal.
 */
class AnalogElement : public Element
{
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

  AnalogElement();

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
  int _pin = A0;

  // map() function just like the standard arduino map() function but with float and boundaries from configuration.
  float mapFloat(int value);

  // map factors
  bool _useMap = false; // use map function when all 4 factors are given.
  int _inMin = 0, _inMax = 0, _outMin = 0, _outMax = 0;

  // configuration of analog input sampling time in ms.
  unsigned long _readTimeMS;

  // next read if analog input value in ms.
  unsigned long _nextReadMS = 0;

  int _value;
  int _hysteresis;

  int _reference;
  int _lastReference;

  /** These actions are sent with the current value. */
  String _valueAction;

  /** These actions are sent with value=1 when the current value is above the reference value otherwise value=0. */
  String _referenceAction;

  /** These actions are sent when the current value is above the reference value. */
  String _highAction;

  /** These actions are sent when the current value is below or equal the reference value. */
  String _lowAction;
};

#ifdef HOMEDING_REGISTER
// Register the AnalogElement onto the ElementRegistry.
bool AnalogElement::registered =
    ElementRegistry::registerElement("analog", AnalogElement::create);
#endif

#endif // ANALOG_H