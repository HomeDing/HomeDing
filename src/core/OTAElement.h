// -----
// OTAElement.h - System Element for handling OTA Updates.
//
// Copyright (c) by Matthias Hertel, https://www.mathertel.de.
//
// This work is licensed under a BSD style license.
// See https://www.mathertel.de/License.aspx.
// More information on https://www.mathertel.de/Arduino
// -----
// 14.04.2018 created by Matthias Hertel
// 27.04.2018 Based on Element class.
// 15.05.2018 set = properties and actions interface.
// 24.09.2018 port and passwd configuration added.
// 10.10.2018 startup as NET element
// -----

#ifndef OTAELEMENT_H
#define OTAELEMENT_H

#include <Arduino.h>

#include "Board.h"
#include "Element.h"

class OTAElement : public Element
{
public:

  /**
   * @brief Factory function to create a OTAElement.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;

  /**
   * @brief Construct a new OTAElement
   */
  OTAElement();

  /**
   * @brief Set a parameter or property to a new value or start an action.
   * @param name Name of property.
   * @param value Value of property.
   * @return true when property could be changed and the corresponding action
   * could be executed.
   */
  virtual bool set(const char *name, const char *value);

  /**
   * @brief Activate the Element.
   * @return true when the Element could be activated.
   * @return false when parameters are not usable.
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
  uint16_t _port = 8266;
  String _passwd;
};

#endif