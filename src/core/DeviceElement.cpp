/**
 * @file DeviceElement.cpp
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
 * Changelog: see DeviceElement.h
 */

#include "DeviceElement.h"
#include <Board.h>
#include <ElementRegistry.h>

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new DeviceElement.
 * @return DeviceElement* created element
 */
Element *DeviceElement::create()
{
  return (new DeviceElement());
} // create()


/* ===== Element functions ===== */

DeviceElement::DeviceElement()
{
  _rebootTime = 0; // no automatic reboot
  startupMode = Element_StartupMode::System;
}


bool DeviceElement::set(const char *name, const char *value)
{
  LOGGER_ETRACE("set(%s:%s)", name, value);
  bool ret = true;

  if (_stricmp(name, "name") == 0) {
    _board->deviceName = value;

  } else if (_stricmp(name, "led") == 0) {
    _board->sysLED = _atopin(value);

  } else if (_stricmp(name, "button") == 0) {
    _board->sysButton = _atopin(value);

  } else if (_stricmp(name, "description") == 0) {
    _description = value;

  } else if (_stricmp(name, "title") == 0) {
    _title = value;

  } else if (_stricmp(name, "room") == 0) {
    _room = value;

  } else if (_stricmp(name, "homepage") == 0) {
    _board->homepage = value;

  } else if (_stricmp(name, "reboottime") == 0) {
    _rebootTime = _atotime(value);

  } else if (_stricmp(name, "logfile") == 0) {
    // enable/disable logfile feature
    Logger::logger_file = _atob(value);

  } else if (_stricmp(name, "loglevel") == 0) {
    // Set global logger loglevel
    Logger::logger_level = _atoi(value);

  } else if (_stricmp(name, "log") == 0) {
    // Log a information with time.
    unsigned long now = _board->getTimeOfDay();
    if (!now)
      now = millis() / 1000; // make seconds
    LOGGER_JUSTINFO(value ? value : "NULL");

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
  unsigned long now = _board->getSeconds();

  if (_rebootTime > 0) {
    _nextBoot = now + _rebootTime;
  } // if

  Element::start();
} // start()


void DeviceElement::loop()
{
  unsigned long now = _board->getSeconds();
  if ((_rebootTime > 0) && (now > _nextBoot)) {
    LOGGER_EINFO("device restart initiated.");
    delay(100);
    ESP.restart();
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
  unsigned long now = _board->getSeconds();

  Element::pushState(callback);
  callback("name", _board->deviceName.c_str());
  callback("description", _description.c_str());
  callback("nextboot", String(_nextBoot - now).c_str());
} // pushState()


// Register the DeviceElement onto the ElementRegistry.
bool DeviceElement::registered =
    ElementRegistry::registerElement("device", DeviceElement::create);

// End
