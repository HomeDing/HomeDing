/**
 * @file BL0937Element.h
 * @brief Element Template class.
 * 
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD 3-Clause style license,
 * https://www.mathertel.de/License.aspx.
 * 
 * More information on https://www.mathertel.de/Arduino
 * 
 * Changelog:
 * * 02.11.2020 created by Matthias Hertel
 */

#ifndef BL0937ELEMENT_H
#define BL0937ELEMENT_H

/**
 * @brief BL0937Element implements...
 * @details
@verbatim

The BL0937Element can ...

@endverbatim
 */

class BL0937Element : public Element
{
public:
  /**
   * @brief Factory function to create a BL0937Element.
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
   * @return true when the Element could be activated.
   * @return false when parameters are not usable.
   */
  virtual void start() override;

  /**
   * @brief Give some processing time to the timer to check for next action.
   */
  virtual void loop() override;

  /**
   * @brief stop all activities and go inactive.
   */
  virtual void term() override;

  /**
   * @brief push the current value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void pushState(
      std::function<void(const char *pName, const char *eValue)> callback) override;

private:
  int _pinSel = -1; // mode selection pin
  int _pinCF = -1; // power pulse pin
  int _pinCF1 = -1; // current/voltage pulse pin

  /* The last detected values */

  unsigned long _powerDuration = 0; // last power duration
  unsigned long _powerCount = 0; // last power impulse count
  unsigned long _powerValue = 0; // last calculated power value

  unsigned long _cf1Duration = 0; // last cf1 duration
  unsigned long _cf1Count = 0; // last cf1 impulse count
  unsigned long _currentValue = 0; // last calculated current value
  unsigned long _voltageValue = 0; // last calculated voltage value

  unsigned long cycleTime = 2 * 1000; // define how long signals will be watched to calculate the sensor values
  unsigned long cycleStart; // start of current cycle
  int voltageMode = LOW; // voltage (HIGH) or current (LOW) mode

  /* Actions */
  String _powerAction; // Actions with power consumption
  String _currentAction; // Actions with current value
  String _voltageAction; // Actions with voltage value
};

#endif