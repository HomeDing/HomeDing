/**
 * @file INA219Element.h
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

#pragma once

#include <sensors/SensorElement.h>

#include "INA219_WE.h"

/**
 * @brief INA219Element implements reading data from an INA219 sensor.
 */

class INA219Element : public SensorElement {
public:
  /**
   * @brief Factory function to create a INA219Element.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;

  /**
   * @brief Construct a new INA219Element
   */
  INA219Element();

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
  /**
   * Sensor specific data, declared in *.cpp
   */
  INA219_WE *_sensor;
  INA219_PGAIN _gain;
  INA219_MEASURE_MODE _mode;
  INA219_BUS_RANGE _range;
  INA219_ADC_MODE _samples;

  /**
   * @brief The _xAction holds the actions that is submitted when ...
   */
  String _voltageAction;
  String _currentAction;
  String _powerAction;
};
