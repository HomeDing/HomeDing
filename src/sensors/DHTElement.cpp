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
#include <Board.h>
#include <Element.h>

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
    if (_stricmp(name, "type") == 0) {
      if (_stricmp(value, "DHT11") == 0) {
        _type = DHTesp::DHT11;
      } else if (_stricmp(value, "DHT22") == 0) {
        _type = DHTesp::DHT22;
      } else if (_stricmp(value, "AUTO") == 0) {
        _type = DHTesp::AUTO_DETECT;
      }
      ret = true;

    } else if (_stricmp(name, PROP_PIN) == 0) {
      _pin = _atopin(value);
      ret = true;

    } else if (_stricmp(name, "ontemperature") == 0) {
      _tempAction = value;
      ret = true;

    } else if (_stricmp(name, "onhumidity") == 0) {
      _humAction = value;
      ret = true;
    } // if
  } // if
  return (ret);
} // set()


/**
 * @brief Activate the DHTElement.
 */
void DHTElement::start()
{
  LOGGER_ETRACE("start()");
  if (_pin < 0) {
    LOGGER_EERR("no meaningful pin");

  } else {
    SensorElement::start();
    _dht.setup(_pin, _type);
  } // if
} // start()


bool DHTElement::getProbe(String &values)
{
  bool newData = false;
  values.clear();
  char buffer[16];

  TempAndHumidity dhtValues;
  int v;

  // LOGGER_ETRACE("reading...");
  dhtValues = _dht.getTempAndHumidity();
  DHTesp::DHT_ERROR_t dhterr = _dht.getStatus();

  if (dhterr == DHTesp::ERROR_TIMEOUT) {
    LOGGER_EERR("timeout");
    term();

  } else if (dhterr == DHTesp::ERROR_CHECKSUM) {
    LOGGER_EERR("checksum");
    term();

  } else {
    // LOGGER_ETRACE("t=%f h=%f", dhtValues.temperature, dhtValues.humidity);
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
  callback("temperature", Element::getItemValue(_values, 0).c_str());
  callback("humidity", Element::getItemValue(_values, 1).c_str());
} // pushState()


// End
