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

#pragma once

#include <HomeDing.h>

/**
 * @brief The SensorElement acts as the base class for sensor elements that need collecting sensor data on a regular basis.
 */
class SensorElement : public Element {
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
  int _valuesCount = 0;  ///< number of values the sensor supports

  String _lastValues;  ///< list of sensor values as "val1,val2"
  unsigned long _startTime;  ///< starting time of a measurement from millis()

  String _stateKeys;  ///< list of keys in the state used for sensor values

  // The actions for value[0], value[1]
  String _actions[4];

  /// set duration for waiting to next communication with the sensor
  virtual void setWait(unsigned long waitMilliseconds);

  /// retrieve values from a sensor
  virtual bool getProbe(String &values);

  /// send data out by crating actions 
  virtual void sendData(String &values);

private:
  /// The time between reading 2 probes. Default Setting: 60 seconds.
  unsigned long _readTime = 60 * 1000;

  /// The current values should be emitted again after some time even when not changing.
  unsigned long _resendTime = 0;

  /// The time to pass before reading a sensor value the first time after Power Up. Default: 3 seconds.
  unsigned long _warmupTime = 3 * 1000;

  bool _restart = false;

  int _state = 0;

  /** remember that the sensor worked at least once so restart with power pin may help */
  bool _sensorWorkedOnce = false;

  unsigned long _waitStart = 0;  ///< time when waiting was started
  unsigned long _waitDuration = 0;  ///< time to wait (milliseconds)

  unsigned long _lastRead = 0;  ///< time of last sensor reading
  unsigned long _nextSend = 0;  ///< time for next value re-sending
};
