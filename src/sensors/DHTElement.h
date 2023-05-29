/**
 * @file DHTElement.h
 *
 * @brief Optional Input Element for the HomeDing Library to read DHT11 and
 * DHT22 sensors and create actions.
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
 * * 21.05.2018 created by Matthias Hertel
 * * 12.02.2020 rebased on SensorElement.
 */

#pragma once

#include <sensors/SensorElement.h>

class DHTNEW;  // forward

/**
 * @brief The DHTElement is an special Element that creates actions based on a
 * digital IO signal.
 */
class DHTElement : public SensorElement {
public:
  /**
   * @brief Factory function to create a DHTElement.
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
   * @brief Activate and power on the DHTElement.
   * @return true when activation was good.
   * @return false when activation failed.
   */
  virtual void start() override;

  /**
   * @brief Stop sensor and power off.
   */
  virtual void term() override;

protected:
  virtual bool getProbe(String &values);

private:
  /**
   * @brief GPIO pin to be used for data transfer to sensor.
   */
  int _pin = -1;

  /**
   * @brief GPIO pin to be used for controlling the power of the sensor.
   */
  int _powerpin = -1;

  /**
   * @brief Control level logic for power ouput to _powerpin. When true: Enable sensor by creating a physical LOW level.
   */
  bool _powerinverse = false;

  DHTNEW *_dht;
};  // class DHTElement


#ifdef HOMEDING_REGISTER
// Register the DHTElement in the ElementRegistry.
bool DHTElement::registered = ElementRegistry::registerElement("dht", DHTElement::create);
#endif
