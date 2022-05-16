/**
 * @file BH1750Element.h
 *
 * @brief Element to create actions based on sensor values from a BH1750 Light sensor.
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
 * * 11.05.2022 created by Matthias Hertel
 */

#pragma once

#include <HomeDing.h>
#include <sensors/SensorElement.h>

#include <Wire.h>

/**
 * @brief The BH1750Element is an special Element to create actions
 * based on sensor values from a SHT20 Temperature and Humidity sensor.
 */
class BH1750Element : public SensorElement {
public:
  /**
   * @brief Factory function to create a BH1750Element.
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
   * @brief Activate the BH1750Element.
   * @return true when activation was good.
   * @return false when activation failed.
   */
  virtual void start() override;

  /**
   * @brief push the current value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void pushState(
    std::function<void(const char *pName, const char *eValue)> callback) override;

protected:
  virtual bool getProbe(String &values);
  virtual void sendData(String &values);

private:
  uint8_t _address = 0x23;  // default i2c address
  uint8_t _mode = 0x20;  // default operating mode: 1 lux resolution, one shot

  /** The actions emitted when a new value was read from the sensor. */
  String _valueAction;

  float _factor = 1.2f;

  unsigned long _dataIsReady;
};

#ifdef HOMEDING_REGISTER
// Register the BH1750Element in the ElementRegistry.
bool BH1750Element::registered =
  ElementRegistry::registerElement("bh1750", BH1750Element::create);
#endif
