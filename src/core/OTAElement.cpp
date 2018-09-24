// -----
// OTAElement.cpp - System Element for handling OTA Updates.
//
// Copyright (c) by Matthias Hertel, https://www.mathertel.de.
//
// This work is licensed under a BSD style license.
// See https://www.mathertel.de/License.aspx.
// More information on https://www.mathertel.de/Arduino
// -----
// Changelog: see OTAElement.h
// -----

#include "OTAElement.h"
#include "ElementRegistry.h"

#include <ArduinoOTA.h>

#define LOGGER_MODULE "OTA"
#include "core/Logger.h"

/**
 * @brief static factory function to create a new OTAElement
 * @return OTAElement* created element
 */
Element *OTAElement::create()
{
  LOGGER_TRACE("create()");
  return (new OTAElement());
} // create()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool OTAElement::set(const char *name, const char *value)
{
  LOGGER_TRACE("set(%s, %s)", name, value);
  bool ret = true;

  if (_stricmp(name, "port") == 0) {
    _port = atoi(value);

  } else if (_stricmp(name, "passwd") == 0) {
    _passwd = value;

  } else {
    ret = Element::set(name, value);
  } // if

  return (ret);
} // set()


/**
 * @brief Activate the OTAElement.
 */
void OTAElement::start()
{
  char devicename[32]; // name of this device on the network
  LOGGER_TRACE("start()");

  // get device name from device Element.
  strncpy(devicename, "homeding", sizeof(devicename));

  Element *deviceElement = _board->getElement("device");
  if (deviceElement)
    strncpy(devicename, deviceElement->get("name"), sizeof(devicename));
  ArduinoOTA.setHostname(devicename);

  ArduinoOTA.setPort(_port); // defaults = 8266
  if (_passwd.length() > 0)
    ArduinoOTA.setPassword((const char *)_passwd.c_str());

  ArduinoOTA.onStart([]() { LOGGER_INFO("Starting"); });
  ArduinoOTA.onEnd([]() { LOGGER_INFO("End."); });

  // The onProgress function is called very often. Only report progress on
  // full percents.
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    static int lastpc = 0;
    int pc = (progress / (total / 100));
    if (pc != lastpc) {
      Serial.print('.');
      if (pc % 50 == 0)
        Serial.printf(" %d %%\n", pc);
      lastpc = pc;
    }
  });

  ArduinoOTA.onError([](ota_error_t error) {
    LOGGER_ERR("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
      LOGGER_ERR("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)
      LOGGER_ERR("Begin Failed");
    else if (error == OTA_CONNECT_ERROR)
      LOGGER_ERR("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)
      LOGGER_ERR("Receive Failed");
    else if (error == OTA_END_ERROR)
      LOGGER_ERR("End Failed");
  });
  ArduinoOTA.begin();
  Element::start();
} // start()


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void OTAElement::loop()
{
  ArduinoOTA.handle();
} // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void OTAElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
} // pushState()

// Register the OTAElement onto the ElementRegistry.
bool OTAElement::registered =
    ElementRegistry::registerElement("ota", OTAElement::create);

// End
