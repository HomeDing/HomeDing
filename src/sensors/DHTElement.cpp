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

/**
 * @brief static factory function to create a new DHTElement
 * @return DHTElement* created element
 */
Element *DHTElement::create()
{
  return (new DHTElement());
} // create()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool DHTElement::set(const char *name, const char *value)
{
  bool ret = SensorElement::set(name, value);

  if (!ret) {
    // Not handled by the SensorElement
    ret = true; // assume
    if (_stricmp(name, "type") == 0) {
      if (_stricmp(value, "DHT11") == 0) {
        _type = DHTesp::DHT11;
      } else if (_stricmp(value, "DHT22") == 0) {
        _type = DHTesp::DHT22;
      } else if (_stricmp(value, "AUTO") == 0) {
        _type = DHTesp::AUTO_DETECT;
      }

    } else if (_stricmp(name, PROP_PIN) == 0) {
      _pin = _atopin(value);

    } else if (_stricmp(name, "powerpin") == 0) {
      // the ouput pin to control powering the component
      _powerpin = _atopin(value);
      ret = true;

    } else if (_stricmp(name, "powerinverse") == 0) {
      _powerinverse = _atob(value);
      ret = true;

    } else if (_stricmp(name, ACTION_ONTEMPERATURE) == 0) {
      _tempAction = value;

    } else if (_stricmp(name, ACTION_ONHUMIDITY) == 0) {
      _humAction = value;

    } else {
      ret = false;
    } // if
  } // if
  return (ret);
} // set()


/**
 * @brief Activate the DHTElement.
 */
void DHTElement::start()
{
  // TRACE("start()");
  if (_pin < 0) {
    LOGGER_EERR("no pin");

  } else {
    SensorElement::start();
    if (_powerpin >= 0) {
      pinMode(_powerpin, OUTPUT);
      int physLevel = (_powerinverse ? LOW : HIGH);
      digitalWrite(_powerpin, physLevel);
    }
    _dht.setup(_pin, _type);
  } // if
} // start()


/**
 * @brief Deactivate the DHTElement.
 */
void DHTElement::term()
{
  // TRACE("term()");
  // no need to call _dht.anyfunc()
  if (_powerpin >= 0) {
    pinMode(_powerpin, OUTPUT);
    int physLevel = (_powerinverse ? HIGH : LOW);
    digitalWrite(_powerpin, physLevel);
  }
  SensorElement::term();
} // start()


bool DHTElement::getProbe(String &values)
{
  bool newData = false;
  char buffer[16];

  TempAndHumidity dhtValues;

  // TRACE("getProbe()");
  dhtValues = _dht.getTempAndHumidity();
  DHTesp::DHT_ERROR_t dhterr = _dht.getStatus();

  if (dhterr == DHTesp::ERROR_TIMEOUT) {
    LOGGER_EERR("timeout");
    term();

  } else if (dhterr == DHTesp::ERROR_CHECKSUM) {
    LOGGER_EERR("checksum");
    term();

  } else {
    // TRACE("t=%f h=%f", dhtValues.temperature, dhtValues.humidity);
    newData = true;
    snprintf(buffer, sizeof(buffer), "%.2f,%.2f", dhtValues.temperature, dhtValues.humidity);
    values = buffer;
  } // if

  return (newData);
} // getProbe()


void DHTElement::sendData(String &values)
{
  _board->dispatchItem(_tempAction, values, 0);
  _board->dispatchItem(_humAction, values, 1);
} // sendData()


void DHTElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  SensorElement::pushState(callback);
  callback("temperature", Element::getItemValue(_lastValues, 0).c_str());
  callback("humidity", Element::getItemValue(_lastValues, 1).c_str());
} // pushState()


// End
