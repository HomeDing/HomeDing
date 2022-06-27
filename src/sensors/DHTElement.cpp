/**
 * @file DHTElement.cpp
 *
 * @brief Optional Input Element for the HomeDing Library to read DHT11 and
 * DHT22 sensors and create actions.
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
 * Changelog: see DHTElement.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <sensors/DHTElement.h>

#include <dhtnew.h>

#define TRACE(...)  // LOGGER_ETRACE(__VA_ARGS__)

/**
 * @brief static factory function to create a new DHTElement
 * @return DHTElement* created element
 */
Element *DHTElement::create() {
  return (new DHTElement());
}  // create()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool DHTElement::set(const char *name, const char *value) {
  bool ret = SensorElement::set(name, value);

  if (!ret) {
    // Not handled by the SensorElement
    ret = true;  // assume
    if (_stricmp(name, "type") == 0) {
      if (_stricmp(value, "DHT11") == 0) {
        _type = 11;
      } else if (_stricmp(value, "DHT22") == 0) {
        _type = 22;
      } else if (_stricmp(value, "AUTO") == 0) {
        _type = 0;
      }

    } else if (_stricmp(name, "pin") == 0) {
      _pin = _atopin(value);

    } else if (_stricmp(name, "powerpin") == 0) {
      // the ouput pin to control powering the component
      _powerpin = _atopin(value);
      ret = true;

    } else if (_stricmp(name, "powerinverse") == 0) {
      _powerinverse = _atob(value);
      ret = true;

    } else if (_stricmp(name, "onTemperature") == 0) {
      _value00Action = value;

    } else if (_stricmp(name, "onHumidity") == 0) {
      _value01Action = value;

    } else {
      ret = false;
    }  // if
  }    // if
  return (ret);
}  // set()


/**
 * @brief Activate the DHTElement.
 */
void DHTElement::start() {
  // TRACE("start()");
  if (_pin < 0) {
    LOGGER_EERR("no pin");

  } else {
    SensorElement::start();
    _valuesCount = 2;
    _stateKeys = "temperature,humidity";

    if (_powerpin >= 0) {
      pinMode(_powerpin, OUTPUT);
      int physLevel = (_powerinverse ? LOW : HIGH);
      digitalWrite(_powerpin, physLevel);
    }
    _dht = new DHTNEW(_pin);
  }  // if
}  // start()


/**
 * @brief Deactivate the DHTElement.
 */
void DHTElement::term() {
  TRACE("term()");
  // no need to call _dht.anyfunc()
  if (_powerpin >= 0) {
    pinMode(_powerpin, OUTPUT);
    int physLevel = (_powerinverse ? HIGH : LOW);
    digitalWrite(_powerpin, physLevel);
  }
  _dht->powerDown();
  delete (_dht);
  SensorElement::term();
}  // start()


bool DHTElement::getProbe(String &values) {
  bool newData = false;

  // TRACE("getProbe()");
  int ret = _dht->read();

  if (ret == DHTLIB_WAITING_FOR_READ) {
    setWait(2000);

  } else if (ret == DHTLIB_OK) {
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%.2f,%.2f", _dht->getTemperature(), _dht->getHumidity());
    values = buffer;
    newData = true;

  } else if (ret == DHTLIB_ERROR_SENSOR_NOT_READY) {
    LOGGER_ETRACE("not found");
    term();

  } else {
    LOGGER_EERR("dht err: %d", ret);
  }  // if

  return (newData);
}  // getProbe()

// End
