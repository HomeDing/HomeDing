/**
 * @file time/DSTimeElement.h
 *
 * @brief Core Element for the HomeDing Library to get the actual local time
 * using a RTC DS3231 chip.
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
 * * 19.01.2019 created by Matthias Hertel
 */

#pragma once

#include <ElementRegistry.h>

/**
 * @brief The DSTimeElement is an special Element that creates actions based on
 * a digital IO signal.
 */
class DSTimeElement : public Element
{
public:
  /**
   * @brief Factory function to create a DSTimeElement.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;

  /**
   * @brief Construct a new DSTimeElement object.
   */
  DSTimeElement();

  /**
   * @brief Set a parameter or property to a new value or start an action.
   * @param name Name of property.
   * @param value Value of property.
   * @return true when property could be changed and the corresponding action
   * could be executed.
   */
  virtual bool set(const char *name, const char *value) override;

  /**
   * @brief Activate the DSTimeElement.
   * @return true when activation was good.
   * @return false when activation failed.
   */
  virtual void start() override;

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

private:
  // time between 2 time synchronizations
  int _readTime;

  /**
   * @brief I2C Device address.
   */
  int _address;

  // 0: no time: 1 receiving time, 2: time ok.
  int _state;

  // when to request a new ntp time from the server
  unsigned long _nextRead;

  // static
  static uint8_t dec2Bcd(uint8_t val);
  static uint8_t bcd2Dec(uint8_t val);

  static void _getDSTime(int adr, struct tm *t);
  static void _setDSTime(int adr, struct tm *t);

  static void _setSystemTime(struct tm *t);

  void _setTime(const char *value);
};

#ifdef HOMEDING_REGISTER
// Register the DSTimeElement onto the ElementRegistry.
bool DSTimeElement::registered =
    ElementRegistry::registerElement("dstime", DSTimeElement::create);
#endif
