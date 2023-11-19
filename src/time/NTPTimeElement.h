/**
 * @file time/NTPTimeElement.h
 *
 * @brief Core Element for the HomeDing Library to get the actual time using the
 * NTP protocol.
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
 * * 05.07.2020 SNTP API adjusted to new standard, see https://github.com/esp8266/Arduino/issues/7392
 */

#pragma once

/**
 * @brief The NTPTimeElement is an special Element that creates actions based on
 * a digital IO signal.
 */
class NTPTimeElement : public Element
{
public:
  /**
   * @brief Factory function to create a NTPTimeElement.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;

  /**
   * @brief Construct a new NTPTimeElement object.
   */
  NTPTimeElement();

  /**
   * @brief Set a parameter or property to a new value or start an action.
   * @param name Name of property.
   * @param value Value of property.
   * @return true when property could be changed and the corresponding action
   * could be executed.
   */
  virtual bool set(const char *name, const char *value) override;

  /**
   * @brief Activate the NTPTimeElement.
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

private:
  // host name of ntp server
  String _ntpServer;

  // local time zone
  String _timezone;
};

#ifdef HOMEDING_REGISTER
// Register the NTPTimeElement onto the ElementRegistry.
bool NTPTimeElement::registered =
    ElementRegistry::registerElement("ntptime", NTPTimeElement::create);
#endif
