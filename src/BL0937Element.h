/**
 * @file BL0937Element.h
 * @brief Element to calculate voltage, current and power consumption using a BL0937 chip.
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

#pragma once

/**
 * @brief BL0937Element implements calculating voltage, current and power consumption using a BL0937 chip.
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
   * @brief Give some processing time check for actions.
   */
  virtual void loop() override;

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

  // ===== default factors taken from a random plug

  float _powerFactor = 1346829.0; // pulse density to power in W
  float _currentFactor = 11177185.0; // pulse density to current in mA
  float _voltageFactor = 129059.0; // pulse density to voltage in V

  unsigned long _cycleTime = 2 * 1000; // define how long signals will be watched to calculate the sensor values
  unsigned long _cycleStart; // start of current cycle
  int _voltageMode = LOW; // voltage (HIGH) or current (LOW) mode

  /* Actions */
  String _powerAction; // Actions with power consumption
  String _currentAction; // Actions with current value
  String _voltageAction; // Actions with voltage value
  String _energyAction; // Actions with energy value of last period (day)
};


/* ===== Register the Element ===== */

#ifdef HOMEDING_REGISTER
// Register the BL0937Element onto the ElementRegistry.
bool BL0937Element::registered =
    ElementRegistry::registerElement("bl0937", BL0937Element::create);
#endif
