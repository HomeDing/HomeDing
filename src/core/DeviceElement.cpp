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

#include <Arduino.h>
#include <HomeDing.h>

#include <core/DeviceElement.h>

#define TRACE(...)  // LOGGER_EINFO(__VA_ARGS__)

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new DeviceElement.
 * @return DeviceElement* created element
 */
Element *DeviceElement::create() {
  return (new DeviceElement());
}  // create()


/* ===== Element functions ===== */

DeviceElement::DeviceElement()
  : _rebootTime(0), _nextBoot(0)  // default with no automatic reboot
{
  startupMode = Element_StartupMode::System;
}


bool DeviceElement::set(const char *name, const char *value) {
  bool ret = true;
  // LOGGER_EERR("set %s='%s'", name, value);

  // ===== actions send to device =====
  if (_stricmp(name, "log") == 0) {
    // Log something
    LOGGER_INFO(value ? value : "NULL");

    // ===== sleep behavior =====

  } else if (_stricmp(name, "sleep") == 0) {
    _board->startSleep();

  } else if (_stricmp(name, "nosleep") == 0) {
    _board->cancelSleep();

    // ===== Web UI =====
  } else if (_stricmp(name, "homepage") == 0) {
    _board->homepage = value;

  } else if (_stricmp(name, "title") == 0) {
    _board->title = value;

  } else if (_stricmp(name, "room") == 0) {
    _board->room = value;

#if defined(ESP8266)
  } else if (_stricmp(name, "outputpower") == 0) {
    _board->outputPower = strtof(value, nullptr);
#endif

  } else if (_stricmp(name, "reboottime") == 0) {
    _rebootTime = _atotime(value);

  } else if (_stricmp(name, "description") == 0) {
    _description = value;  // is used in SSDP

  } else if (_stricmp(name, "loglevel") == 0) {
    // Set global logger loglevel
    Logger::logger_level = _atoi(value);

  } else if (_stricmp(name, "logfile") == 0) {
    // enable/disable logfile feature
    Logger::setLogFile(_atob(value));

  } else if (_stricmp(name, "reset") == 0) {
    // reboot is called reset in ESP
    _board->reboot(false);

  } else {
    ret = Element::set(name, value);
  }

  if ((!ret) && (!active)) {
    // These actions are only allowed during setup / before activation.

    if (_stricmp(name, "name") == 0) {
      _board->deviceName = value;

    } else if (_stricmp(name, "safemode") == 0) {
      _board->isSafeMode = _atob(value);

    } else if (_stricmp(name, "savemode") == 0) {
      LOGGER_EERR("use safemode parameter");
      _board->isSafeMode = _atob(value);

    } else if (_stricmp(name, "onSysStart") == 0) {
      _board->sysStartAction = value;

    } else if (_stricmp(name, "onStart") == 0) {
      _board->startAction = value;

    } else if (_stricmp(name, "cache") == 0) {
      _board->cacheHeader = value;

      // ===== Service Discovery =====
    } else if (_stricmp(name, "sd") == 0) {
      _board->_mDnsEnabled = _atob(value);

      // ===== WiFi Manager =====

    } else if (_stricmp(name, "led") == 0) {
      _board->sysLED = _atopin(value);

    } else if (_stricmp(name, "button") == 0) {
      _board->sysButton = _atopin(value);

    } else if (_stricmp(name, "connecttime") == 0) {
      _board->maxNetConnextTime = _scanDuration(value);

    } else if (_stricmp(name, "sleeptime") == 0) {
      _board->setSleepTime(_scanDuration(value));

      // ===== I2C bus =====

    } else if (_stricmp(name, "i2c-sda") == 0) {
      _board->I2cSda = _atopin(value);

    } else if (_stricmp(name, "i2c-scl") == 0) {
      _board->I2cScl = _atopin(value);

    } else if (_stricmp(name, "i2c-frequency") == 0) {
      _board->I2cFrequency = _atoi(value);

    }  // if
  }    // if (! active)

  return (ret);
}  // set()


/**
 * @brief Activate the DeviceElement.
 */
void DeviceElement::start() {
  unsigned long now = _board->getSeconds();

  if (_rebootTime > 0) {
    _nextBoot = now + _rebootTime;
  }  // if

  Element::start();
}  // start()


void DeviceElement::loop() {
  TRACE("loop()");
  unsigned long now = _board->getSeconds();
  if ((_rebootTime > 0) && (now > _nextBoot)) {
    LOGGER_EINFO("restart initiated");
    delay(100);
    ESP.restart();
    delay(100);
  }  // if
}  // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void DeviceElement::pushState(
  std::function<void(const char *pName, const char *eValue)> callback) {
  unsigned long now = _board->getSeconds();

  Element::pushState(callback);
  callback("name", _board->deviceName.c_str());
  callback("title", _board->title.c_str());
  callback("description", _description.c_str());
  callback("safemode", _printBoolean(_board->isSafeMode));
  callback("sd", _printBoolean(_board->_mDnsEnabled));
  callback("nextboot", _printInteger(_nextBoot - now));
}  // pushState()


// Always register the DeviceElement onto the ElementRegistry.
bool DeviceElement::registered =
  ElementRegistry::registerElement("device", DeviceElement::create);

// End
