/**
 * @file INA226Element.h
 * @brief Element to control a INA219 power consumption chip.
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
 * * 12.08.2020 created by Matthias Hertel
 * * 31.08.2021 finalized for power sensor example
 */

#ifndef INA226ELEMENT_H
#define INA226ELEMENT_H

#include <sensors/SensorElement.h>

#include "INA226_WE.h"

/**
 * @brief INA226Element implements reading data from an INA266 sensor.
 */

class INA226Element : public SensorElement {
public:
  /**
   * @brief Factory function to create a INA226Element.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;

  /**
   * @brief Construct a new INA226Element
   */
  INA226Element();

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
   * @brief stop all activities and go inactive.
   */
  virtual void term() override;

  /**
   * @brief push the current value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void pushState(
      std::function<void(const char *pName, const char *eValue)> callback) override;

protected:
  // SensorElement functions

  // try to get the next probe
  virtual bool getProbe(String &values);

  // emit actions using latest data
  virtual void sendData(String &values);

private:
  /** I2C address */
  int _address;

  /**
   * Sensor specific data, declared in *.cpp 
   */
  INA226_WE *_sensor;
  INA226_MEASURE_MODE _mode;
  float _shunt;
  float _cRange;
  INA226_CONV_TIME _convTime;
  INA226_AVERAGES _averageMode;

  /**
   * @brief The _xAction holds the actions that is submitted when ...
   */
  String _voltageAction;
  String _currentAction;
  String _powerAction;
};

#endif