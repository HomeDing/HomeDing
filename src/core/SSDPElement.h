
// -----
// SSDPElement.h - Element for specifying Element discovery using SSDP.
// Enable the SSDP Protocol to find the device in the network
//
// Copyright (c) by Matthias Hertel, https://www.mathertel.de.
//
// This work is licensed under a BSD style license.
// See https://www.mathertel.de/License.aspx.
// More information on https://www.mathertel.de/Arduino
// -----
// 17.06.2018 created by Matthias Hertel
// -----

#ifndef SSDPELEMENT_H
#define SSDPELEMENT_H

#include <Arduino.h>
#include <Element.h>

/// enable the SSDP Protocol to find the device in the network

#include <ESP8266SSDP.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>


/**
 * @brief Element for enabling Element discovery using the SSDP protocol.
 * @details
@verbatim

SSDP...

@endverbatim
 */

class SSDPElement : public Element
{
public:
  /**
   * @brief Factory function to create a SSDPElement.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;

  /**
   * @brief initialize a new Element.
   * @param board The board reference.
   */
  virtual void init(Board *board);

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

  // loop() is not required because no local activities
  // pushState() is not required because no dynamic properties
  // term() is not required because no terminateion functionality

private:

};

#endif