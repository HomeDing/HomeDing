// -----
// DeviceElement.cpp - Library for creating a digital outgoing signal.
//
// Copyright (c) by Matthias Hertel, https://www.mathertel.de.
//
// This work is licensed under a BSD style license.
// See https://www.mathertel.de/License.aspx.
// More information on https://www.mathertel.de/Arduino
// -----
// Changelog: see DeviceElement.h
// -----

// https://stackoverflow.com/questions/18806141/move-object-creation-to-setup-function-of-arduino

#include "DeviceElement.h"
#include "ElementRegistry.h"

#define LOGGER_MODULE "Device"
#include "Logger.h"

#include <SSD1306Wire.h>

/**
 * @brief static factory function to create a new DeviceElement.
 * @return DeviceElement* created element
 */
Element *DeviceElement::create()
{
  LOGGER_TRACE("create()");
  return (new DeviceElement());
} // create()


DeviceElement::DeviceElement()
{
  _rebootTime = 0; // no automatic reboot
}


bool DeviceElement::set(const char *name, const char *value)
{
  LOGGER_INFO("set(%s:%s)", name, value);
  bool ret = true;

  if (_stricmp(name, "name") == 0) {
    _deviceName = value;

  } else if (_stricmp(name, "reboottime") == 0) {
    _rebootTime = _atotime(value);

  } else if (_stricmp(name, "log") == 0) {
    // Log a information with time.
    unsigned long now = millis() / 1000; // make seconds

    // ALWAYS send to log
    // LOGGER_INFO(...
    DEBUG_ESP_PORT.printf("%3u:%02u:%02u %s, %d\n", (now / 3600),
                          (now / 60) % 60, now % 60, value ? value : "NULL",
                          ESP.getFreeHeap());

  } else {
    ret = Element::set(name, value);
  } // if

  return (ret);
} // set()


/**
 * @brief Activate the DeviceElement.
 */
void DeviceElement::start()
{
  unsigned long now = millis() / 1000;
  if (_rebootTime > 0) {
    _nextBoot = now + _rebootTime;
  } // if

  Element::start();
} // start()


void DeviceElement::loop()
{
  unsigned long now = millis() / 1000;
  if ((_rebootTime > 0) && (now > _nextBoot)) {
    LOGGER_INFO("reboot");
    delay(100);
    ESP.reset();
    delay(100);

    // ESP.deepSleep(8000);
    // abort();
    // ESP.reset();

  } // if
} // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void DeviceElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  unsigned long now = millis() / 1000;

  Element::pushState(callback);
  callback("name", _deviceName.c_str());
  callback("nextboot", String(_nextBoot - now).c_str());
} // pushState()


// Register the DeviceElement onto the ElementRegistry.
bool DeviceElement::registered =
    ElementRegistry::registerElement("device", DeviceElement::create);

// End
