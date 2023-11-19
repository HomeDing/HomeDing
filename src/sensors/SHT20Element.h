/**
 * @file SHT20Element.h
 * 
 * @brief Element to create actions based on sensor values from a SHT20 Temperature and Humidity sensor.
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
 * * 27.11.2020 created by Matthias Hertel
 */

#pragma once

#include <sensors/SensorElement.h>

#include <Wire.h>

/**
 * @brief The SHT20Element is an special Element to create actions
 * based on sensor values from a SHT20 Temperature and Humidity sensor.
 */
class SHT20Element : public SensorElement
{
public:
  /**
   * @brief Factory function to create a SHT20Element.
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
   * @brief Activate the SHT20Element.
   * @return true when activation was good.
   * @return false when activation failed.
   */
  virtual void start() override;

protected:
  virtual bool getProbe(String &values);

private:
  uint8_t _address = 0x40; // default i2c address
  uint8_t _state = 0; 
  unsigned long _maxTime;

  uint8_t _crc8(uint8_t *data, int length);

// try read some bytes and return as 16-bit. 0 is error.
  uint16_t _read();

  float _temperature;
  float _humidity;
};

#ifdef HOMEDING_REGISTER
// Register the SHT20Element in the ElementRegistry.
bool SHT20Element::registered = ElementRegistry::registerElement("sht20", SHT20Element::create);
#endif
