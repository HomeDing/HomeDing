/**
 * @file DallasElement .h
 *
 * @brief Sensor Element for the HomeDing Library to read DS18B20 and other OneLine sensors supported 
 * by the DallasTemperature library and create actions.
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
 * * 07.05.2022 can read more Dallas Temp Sensors using DallasTemperature library.
 *              renamed from DS18B20Element to DallasElement.
 */

#pragma once

#include <HomeDing.h>
#include <sensors/SensorElement.h>

/**
 * @brief The DallasElement  is an special Element that creates actions based on a
 * digital IO signal.
 */
class DallasElement : public SensorElement {

public:
  /**
   * @brief Factory function to create a DallasElement .
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;

  /**
   * @brief Construct a new DallasElement
   */
  DallasElement();

  /**
   * @brief Set a parameter or property to a new value or start an action.
   * @param name Name of property.
   * @param value Value of property.
   * @return true when property could be changed and the corresponding action
   * could be executed.
   */
  virtual bool set(const char *name, const char *value) override;

  /**
   * @brief Activate the DallasElement .
   * @return true when activation was good.
   * @return false when activation failed.
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
  // implementation details
  class DallasElementImpl *_impl;
};


#ifdef HOMEDING_REGISTER
// Register the DallasElement  in the ElementRegistry.
bool DallasElement ::registered =
  ElementRegistry::registerElement("dallas", DallasElement ::create);
#endif
