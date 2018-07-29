// -----
// SSDPElement.cpp - System Element for specifying Element discovery using SSDP.
// Enable the SSDP Protocol to find the device in the network.
//
// Copyright (c) by Matthias Hertel, https://www.mathertel.de.
//
// This work is licensed under a BSD style license.
// See https://www.mathertel.de/License.aspx.
// More information on https://www.mathertel.de/Arduino
// -----
// Changelog: see SSDPElement.h
// -----

#include "SSDPElement.h"
#include "ElementRegistry.h"

#define LOGGER_MODULE "SSDP"
#include "Logger.h"


/**
 * @brief static factory function to create a new SSDPElement
 * @return SSDPElement* created element
 */
Element *SSDPElement::create()
{
  LOGGER_TRACE("create()");
  return (new SSDPElement());
} // create()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool SSDPElement::set(const char *name, const char *value)
{
  LOGGER_TRACE("set(%s, %s)", name, value);
  bool ret = true;

  /*
    if (_stricmp(name, "type") == 0) {
      _cycleTime = _atotime(value);

    } else if (_stricmp(name, "onon") == 0) {
      _onAction = value;

    } else {
      ret = Element::set(name, value);
    } // if
  */

  ret = Element::set(name, value);
  return (ret);
} // set()


/**
 * @brief Activate the SSDPElement.
 */
void SSDPElement::start()
{
  LOGGER_TRACE("start()");

  Element *deviceElement = _board->getElement("device");

  if ((deviceElement) && (_board->server)) {
    // get devicename and description from the device Element
    SSDP.setSchemaURL("description.xml");
    SSDP.setHTTPPort(80);
    SSDP.setName(deviceElement->get("name"));
    SSDP.setSerialNumber(ESP.getChipId());
    SSDP.setURL("/");
    SSDP.setModelName(deviceElement->get("description"));
    SSDP.setModelNumber("v0004");
    SSDP.setModelURL("http://www.mathertel.de/Arduino/Board");
    SSDP.setManufacturer("Matthias Hertel");
    SSDP.setManufacturerURL("https://www.mathertel.de");
    SSDP.setDeviceType("upnp:rootdevice");
    SSDP.begin();
    ESP8266WebServer *server = _board->server;
    server->on("/description.xml", HTTP_GET,
               [server]() { SSDP.schema(server->client()); });
    Element::start();
  } // if
} // start()


/**
 * @brief stop all activities of the timer and go inactive.
 */
void SSDPElement::term()
{
  Element::term();
} // term()


// Always register the SSDPElement in the ElementRegistry.
bool SSDPElement::registered =
    ElementRegistry::registerElement("ssdp", SSDPElement::create);

// End
