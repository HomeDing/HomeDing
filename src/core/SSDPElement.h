
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
// 23.01.2022 startup after network. 
// -----

#ifndef SSDPELEMENT_H
#define SSDPELEMENT_H

#include <HomeDing.h>

#if defined(ESP8266)
#include <ESP8266SSDP.h>
#elif defined(ESP32)
#include <ESP32SSDP.h>
#endif

/**
 * @brief Element for enabling Element discovery using the SSDP protocol.
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
  virtual void init(Board *board) override;

  /**
   * @brief Set a parameter or property to a new value or start an action.
   * @param name Name of property.
   * @param value Value of property.
   * @return true when property could be changed and the corresponding action
   * could be executed.
   */
  virtual bool set(const char *name, const char *value) override;

  /**
   * @brief Activate the Element.
   */
  virtual void start() override;

  // loop() is not required because no local activities
  // pushState() is not required because no dynamic properties
  // term() is not required because no terminateion functionality

private:
  void _sendSchema();
};

#ifdef HOMEDING_REGISTER
// Register the SSDPElement in the ElementRegistry
bool SSDPElement::registered =
    ElementRegistry::registerElement("ssdp", SSDPElement::create);
#endif

#endif