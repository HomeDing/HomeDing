/**
 * @file SCD4XElement.h
 *
 * @brief Element to create actions based on sensor values from a SCD40 and SCD41 CO2, Temperature and Humidity sensor.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD 3-Clause style license,
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino.
 *
 * Changelog:
 * * 27.06.2022 created by Matthias Hertel
 */

#pragma once

#include <sensors/SensorElement.h>

#include <Wire.h>

/**
 * @brief The SCD4XElement is an special Element to create actions
 * based on sensor values from a SHT20 Temperature and Humidity sensor.
 */
class SCD4XElement : public SensorElement {
public:
  enum SCANMODE : int16_t {
    CONTINUOUS = 0,  // startup operation states
    LOWPOWER = 1,    // load configurations and create elements. Start SYS
    SINGLE = 2,      // unspecified
  };

  /**
   * @brief Factory function to create a SCD4XElement.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;

  SCD4XElement();

  /**
   * @brief Set a parameter or property to a new value or start an action.
   * @param name Name of property.
   * @param value Value of property.
   * @return true when property could be changed and the corresponding action
   * could be executed.
   */
  virtual bool set(const char *name, const char *value) override;

  /**
   * @brief Activate the SCD4XElement.
   * @return true when activation was good.
   * @return false when activation failed.
   */
  virtual void start() override;

  /**
   * @brief push the current value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  // virtual void pushState(std::function<void(const char *pName, const char *eValue)> callback) override;

protected:
  virtual bool getProbe(String &values);
  // virtual void sendData(String &values);

private:
  uint8_t _address = 0x62;  // default i2c address
  uint8_t _state = 0;
  SCANMODE _mode = SCANMODE::CONTINUOUS;

  static uint8_t _crc8(uint8_t *data, int len);
};

#ifdef HOMEDING_REGISTER
// Register the SCD4XElement in the ElementRegistry.
bool SCD4XElement::registered = ElementRegistry::registerElement("scd4x", SCD4XElement::create);
#endif
