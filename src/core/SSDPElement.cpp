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

#include <Arduino.h>
#include <HomeDing.h>

#include <core/SSDPElement.h>

#if defined(ESP8266)
#include <ESP8266SSDP.h>
#elif defined(ESP32)
#include <ESP32SSDP.h>
#endif

#define TRACE(...) // LOGGER_JUSTINFO(__VA_ARGS__)

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new SSDPElement
 * @return SSDPElement* created element
 */
Element *SSDPElement::create() {
  return (new SSDPElement());
} // create()


/* ===== Element functions ===== */

/**
 * @brief initialize the common functionality of all element objects.
 */
void SSDPElement::init(Board *board) {
  Element::init(board);

  Element *deviceElement = board->getElement("device");

  if ((deviceElement) && (board->server)) {
    // part of the notification response
    SSDP.setHTTPPort(80);
    SSDP.setModelName(board->deviceName);
    SSDP.setModelNumber("0.5.0"); // HomeDing Library Version
    SSDP.setDeviceType("upnp:rootdevice");
    SSDP.setUUID("upnp:rootdevice");
    SSDP.setSchemaURL("ssdp.xml"); // url to get the SSDP detailed information

    // set default values
#if defined(ESP8266)
    SSDP.setSerialNumber(ESP.getChipId());
#elif defined(ESP32)
    // SSDP.setSerialNumber(ESP.getEfuseMac());
#endif

    SSDP.setURL("/");
    SSDP.setModelURL("https://homeding.github.io/");
    SSDP.setManufacturer("Matthias Hertel");
    SSDP.setManufacturerURL("https://www.mathertel.de");
    SSDP.setName(board->deviceName);

    WebServer *server = board->server;
    server->on("/ssdp.xml", HTTP_GET, [server]() {
#if defined(ESP8266)
      SSDP.schema(server->client());
#elif defined(ESP32)
      server->send(200, "text/plain", SSDP.getSchema());
#endif
    });
  } // if

} // init()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool SSDPElement::set(const char *name, const char *value) {
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


/** @brief Activate the Element. */
void SSDPElement::start() {
  Element::start();
  SSDP.begin();
};

// End
