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
 */

#ifndef BME680ELEMENT_H
#define BME680ELEMENT_H

#include <Arduino.h>
#include <Element.h>

#include <Adafruit_BME680.h>

/**
 * @brief BME680Element implements...
 * @details
@verbatim

The BME680Element can ...

@endverbatim
 */

class BME680Element : public Element
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
   * @brief initialize a new Element.
   * @param board The board reference.
   */
  // virtual void init(Board *board);

  /**
   * @brief Set a parameter or property to a new value or start an action.
   * @param name Name of property.
   * @param value Value of property.
   * @return true when property could be changed and the corresponding action
   * could be executed.
   */
  virtual bool set(const char *name, const char *value);

  /**
   * @brief Activate the Element.
   * @return true when the Element could be activated.
   * @return false when parameters are not usable.
   */
  virtual void start();

  /**
   * @brief Give some processing time to the timer to check for next action.
   */
  virtual void loop();

  /**
   * @brief push the current value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void pushState(
      std::function<void(const char *pName, const char *eValue)> callback);

private:
  void _newValue(String &strVal, const char *fmt, float value, String &actions);

  /** @brief The last reported /actual values of the sensor.
   * The values are formatted as strings as they are formatted in the action
   * urls.
   */
  String _temperature;
  String _humidity;
  String _pressure;
  String _gas;

  /**
   * @brief state in reading values from the sensor.
   * 0: wait for next time
   * 1: reading initiated, wait for _endTime
   */
  int _state;

  int _address = BME680_DEFAULT_ADDRESS;

  /**
   * @brief Time between 2 sensor readings.
   */
  unsigned long _readTime = 60;

  /**
   * @brief time of next sensor reading.
   */
  unsigned long _nextRead;

  /**
   * @brief time (millis) when sensor data is available.
   */
  unsigned long _endTime;

  // sensor reading library
  Adafruit_BME680 _bme;

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

  /**
   * @brief The actions emitted when a new pressure value was read from the
   * sensor.
   */
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