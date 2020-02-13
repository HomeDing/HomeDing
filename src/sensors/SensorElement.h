/**
 * @file SensorElement.h
 * 
 * @brief This Input Element acts as the base class for some sensor elements that need collecting sensor data on a regular basis.
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
 * * 12.02.2020 created by Matthias Hertel from DHT Element implemenation.
 */

#ifndef SENSORELEMENT_H
#define SENSORELEMENT_H

#include <HomeDing.h>

/**
 * @brief The SensorElement acts as the base class for some sensor elements that need collecting sensor data on a regular basis.
 */
class SensorElement : public Element
{
public:
  /**
   * @brief Construct a new SensorElement object.
   */
  SensorElement();

  /**
   * @brief Set a parameter or property to a new value or start an action.
   * @param name Name of property.
   * @param value Value of property.
   * @return true when property could be changed and the corresponding action
   * could be executed.
   */
  virtual bool set(const char *name, const char *value);

  /**
   * @brief Activate the SensorElement.
   * @return true when activation was good.
   * @return false when activation failed.
   */
  virtual void start();

  /**
   * @brief check the state of the DHT values and eventually create actions.
   */
  virtual void loop();

  /**
   * @brief push the current value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void pushState(
      std::function<void(const char *pName, const char *eValue)> callback);

protected:
  virtual unsigned long _startSensor();
  virtual bool _readSensorData();
  virtual void _sendSensorData();

private:
  /**
   * @brief The current values should be emitted again after some time even when not changing.
   */
  int _resendTime;

  /**
   * @brief The time between reading 2 probes.
   */
  unsigned long _readTime;

  unsigned long _nextStart;
  unsigned long _nextRead;
  unsigned long _nextResend;
};


#endif // SENSORELEMENT_H