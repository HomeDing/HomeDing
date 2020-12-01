/**
 * @file BME680Element.h
 * @brief Element for reading BME680 sensor data.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license.
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:
 * * 15.02.2019 created by Matthias Hertel
 * * 12.02.2020 rebased on SensorElement.
 * * 15.04.2020 switch to original bosch https://github.com/BoschSensortec/BME680_driver
 *              no dependency on Adafruit Unified Sensor and Adafruit BME680 Library.
 */

#ifndef BME680ELEMENT_H
#define BME680ELEMENT_H

#include <HomeDing.h>
#include <sensors/SensorElement.h>

#include <sensors/bme680.h>

/**
 * @brief BME680Element implements...
 * @details
@verbatim

The BME680Element can ...

@endverbatim
 */

class BME680Element : public SensorElement
{
public:
  /**
   * @brief Factory function to create a BME680Element.
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
   * @brief push the current value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void pushState(
      std::function<void(const char *pName, const char *eValue)> callback) override;

protected:
  virtual bool getProbe(String &values);
  virtual void sendData(String &values);

private:
  unsigned long beginReading(void);

  /**
   * @brief state in reading values from the sensor.
   * 0: wait for next time
   * 1: reading initiated, wait for _endTime
   */
  unsigned long _dataAvailable;

  int _address = BME680_I2C_ADDR_SECONDARY; // BME680 I2C addresses are 0x76 or 0x77

  /**
   * @brief The actions emitted when a new temperature value was read from the
   * sensor.
   */
  String _temperatureAction;

  /**
   * @brief The actions emitted when a new humidity value was read from the
   * sensor.
   */
  String _humidityAction;

  /** The actions emitted when a new pressure value was read from the sensor. */
  String _pressureAction;

  /**
   * @brief The actions emitted when a new gas value was read from the sensor.
   */
  String _gasAction;
};


#ifdef HOMEDING_REGISTER
// Register the DHTElement in the ElementRegistry.
bool BME680Element::registered =
    ElementRegistry::registerElement("bme680", BME680Element::create);
#endif

#endif