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
  virtual bool set(const char *name, const char *value) override;

  /**
   * @brief Activate the SensorElement.
   * @return true when activation was good.
   * @return false when activation failed.
   */
  virtual void start() override;

  /**
   * @brief Stop sensor and power off.
   */
  virtual void term() override;

  /**
   * @brief check the state of the DHT values and eventually create actions.
   */
  virtual void loop() override;

  /**
   * @brief push the current value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void pushState(
      std::function<void(const char *pName, const char *eValue)> callback) override;

protected:
  String _lastValues;

  virtual bool getProbe(String &values);
  virtual void sendData(String &values);

private:
  /**
   * @brief The time between reading 2 probes.
   */
  unsigned long _readTime;

  /**
   * @brief The current values should be emitted again after some time even when not changing.
   */
  unsigned long _resendTime;

  /**
   * @brief The time to pass before reading a sensor value.
   */
  unsigned long _warmupTime;

  bool _restart;

  /**
   * @brief is set to true while in the getProbe function to distinguish term() calls internally and externally.
   */
  bool _isReading;

  bool _sensorWorkedOnce;

  unsigned long _nextRead;
  unsigned long _nextSend;
};


#endif // SENSORELEMENT_H