/**
 * @file AM2320Element.h
 *
 * @brief Element to create actions based on sensor values from a AM2320 Temperature and Humidity sensor.
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
 * * 27.05.2022 created by Matthias Hertel
 */

#pragma once

#include <HomeDing.h>
#include <sensors/SensorElement.h>

#include <Wire.h>

/**
 * @brief The AM2320Element is an special Element to create actions
 * based on sensor values from a AM2320 Temperature and Humidity sensor.
 */
class AM2320Element : public SensorElement {
public:
  /**
   * @brief Factory function to create a AM2320Element.
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
   * @brief Activate the AM2320Element.
   * @return true when activation was good.
   * @return false when activation failed.
   */
  virtual void start() override;

protected:
  virtual bool getProbe(String &values);

private:
  uint8_t _address = 0x5C;  // default i2c address
  uint8_t _state = 0;

  static uint16_t _crc16(uint8_t *ptr, int len);
};

#ifdef HOMEDING_REGISTER
// Register the AM2320Element in the ElementRegistry.
bool AM2320Element::registered = ElementRegistry::registerElement("am2320", AM2320Element::create);
#endif
