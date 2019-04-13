/**
 * @file DeviceElement.h
 *
 * @brief System Element for the HomeDing Library to manage device level
 * settings.
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
 * * 04.05.2018 created by Matthias Hertel
 * * 24.09.2018 description added.
 * * 10.10.2018 startup as SYS element
 */

#ifndef DEVICEELEMENT_H
#define DEVICEELEMENT_H

#include <Arduino.h>
#include <Element.h>

/**
 * @brief The Device is an Element that offers some geeral and useful
 * functionality required to build IoT Things.
 */
class DeviceElement : public Element
{
public:
  /* ===== Static factory function ===== */

  /**
   * @brief Factory function to create a DeviceElement.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;

  /* ===== Element functions ===== */

  /**
   * @brief Constructor of a new DeviceElement.
   */
  DeviceElement();

  /**
   * @brief Set a parameter or property to a new value or start an action.
   * @param name Name of property.
   * @param value Value of property.
   * @return true when property could be changed and the corresponding action
   * could be executed.
   */
  virtual bool set(const char *name, const char *value);

  /**
   * @brief Activate the DeviceElement.
   * @return true when activation was good.
   * @return false when activation failed.
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
  /**
   * @brief Title of the device for usage in Web UI.
   */
  String _title;

  /**
   * @brief Description of the device for usage in Web UI.
   */
  String _description;

  /**
   * @brief Name of the room of the device.
   */
  String _room;

  /**
   * @brief hours to wait until a fresh reboot will be started,
   */
  uint16_t _rebootTime;

  /**
   * @brief time when the server will be rebooted and have a fresh register in
   * the network.
   */
  unsigned long _nextBoot;
};

#endif // DEVICEELEMENT_H