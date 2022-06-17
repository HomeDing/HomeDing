/**
 * @file BMP280Element .h
 *
 * @brief Optional Input Element for the HomeDing Library to read DS18B20 sensors and create actions.
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
 * * 14.02.2020 created by Matthias Hertel
 */

#pragma once

#include <HomeDing.h>
#include <sensors/SensorElement.h>

#include <Wire.h>

/**
 * @brief The BMP280Element  is an special Element that creates actions based on a
 * digital IO signal.
 */
class BMP280Element : public SensorElement {
public:
  /**
   * @brief Factory function to create a BMP280Element .
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
   * @brief Activate the BMP280Element .
   * @return true when activation was good.
   * @return false when activation failed.
   */
  virtual void start() override;

protected:
  virtual bool getProbe(String &values);

private:
  uint8_t _address = 0x76;  // default BMP280 I2C address
  uint8_t _state = 0;       // 0: sensor not started, 1: sensor was started

  // temperature calibration factors
  uint16_t dig_T1;
  int16_t dig_T2;
  int16_t dig_T3;

  // pressure calibration factors
  uint16_t dig_P1;
  int16_t dig_P2;
  int16_t dig_P3;
  int16_t dig_P4;
  int16_t dig_P5;
  int16_t dig_P6;
  int16_t dig_P7;
  int16_t dig_P8;
  int16_t dig_P9;

  int32_t t_fine;  // temperature required to calculate compensated pressure.

  float BMP280CompensateTemperature(int32_t adc_T);
  float BMP280CompensatePressure(int32_t adc_P);
};

#ifdef HOMEDING_REGISTER
// Register the BMP280Element  in the ElementRegistry.
bool BMP280Element::registered = ElementRegistry::registerElement("bmp280", BMP280Element::create);
#endif
