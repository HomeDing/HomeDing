/**
 * @file AHT20Element.h
 * 
 * @brief Element to create actions based on sensor values from a AHT20 Temperature and Humidity sensor.
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
 * * 12.06.2022 created by Matthias Hertel
 */

#pragma once

#include <sensors/SensorElement.h>

#include <Wire.h>

/**
 * @brief The AHT20Element is an special Element to create actions
 * based on sensor values from a SHT20 Temperature and Humidity sensor.
 */
class AHT20Element : public SensorElement
{
public:
  /**
   * @brief Factory function to create a AHT20Element.
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
   * @brief Activate the AHT20Element.
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
  uint8_t _address = 0x38; // default i2c address
  uint8_t _state = 0; 

  static uint8_t _crc8(uint8_t *data, int len);
};

#ifdef HOMEDING_REGISTER
// Register the AHT20Element in the ElementRegistry.
bool AHT20Element::registered = ElementRegistry::registerElement("aht20", AHT20Element::create);
#endif
