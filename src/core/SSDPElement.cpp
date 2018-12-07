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

#include <core/SSDPElement.h>
#include <ElementRegistry.h>
#include <Board.h>


/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new SSDPElement
 * @return SSDPElement* created element
 */
Element *SSDPElement::create()
{
  return (new SSDPElement());
} // create()


/* ===== Element functions ===== */

/**
 * @brief initialize the common functionality of all element objects.
 */
void SSDPElement::init(Board *board)
{
  LOGGER_ETRACE("init()");
  Element::init(board);

  // set default values
  SSDP.setSchemaURL("$desc.xml");
  SSDP.setHTTPPort(80);
  SSDP.setSerialNumber(ESP.getChipId());
  SSDP.setURL("/");
  SSDP.setModelNumber(__DATE__);
  SSDP.setModelURL("http://www.mathertel.de/Arduino/Board");
  SSDP.setManufacturer("Matthias Hertel");
  SSDP.setManufacturerURL("https://www.mathertel.de");
  SSDP.setDeviceType("upnp:rootdevice");
} // init()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool SSDPElement::set(const char *name, const char *value)
{
  LOGGER_ETRACE("set(%s, %s)", name, value);
  bool ret = true;

  if (_stricmp(name, "Manufacturer") == 0) {
    SSDP.setManufacturer(value);

  } else if (_stricmp(name, "ManufacturerURL") == 0) {
    SSDP.setManufacturerURL(value);

  } else if (_stricmp(name, "ModelURL") == 0) {
    SSDP.setModelURL(value);

  } else if (_stricmp(name, "ModelNumber") == 0) {
    SSDP.setModelNumber(value);

  } else {
    ret = Element::set(name, value);
  } // if

  return (ret);
} // set()


/**
 * @brief Activate the SSDPElement.
 */
void SSDPElement::start()
{
  LOGGER_ETRACE("start()");

  Element *deviceElement = _board->getElement("device");
  if ((deviceElement) && (_board->server)) {
    SSDP.setName(deviceElement->get("name"));
    SSDP.setModelName(deviceElement->get("description"));
    SSDP.begin();
    ESP8266WebServer *server = _board->server;
    server->on("/$desc.xml", HTTP_GET,
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
