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
  unsigned int now = _board->getSeconds();
  if (_pin < 0) {
    LOGGER_EERR("no meaningful pin");

  } else {
    _lastTemp = _lastHum = -666;
    SensorElement::start();
    _dht.setup(_pin, _type);
    _lastTemp = _lastHum = -666; // force to send out the values
  } // if
} // start()


bool DHTElement::_readSensorData()
{
  bool newData = false;

  TempAndHumidity values;
  int v;

  // LOGGER_ETRACE("reading...");
  values = _dht.getTempAndHumidity();
  DHTesp::DHT_ERROR_t dhterr = _dht.getStatus();

  // LOGGER_ETRACE("t=%f h=%f", values.temperature, values.humidity);
  if (dhterr == DHTesp::ERROR_TIMEOUT) {
    LOGGER_EERR("timeout");

  } else if (dhterr == DHTesp::ERROR_CHECKSUM) {
    LOGGER_EERR("checksum");

  } else {
    v = (int)(values.temperature * 100);
    if (v != _lastTemp) {
      newData = true;
      _lastTemp = v;
    }

    v = (int)(values.humidity * 100);
    if (v != _lastHum) {
      newData = true;
      _lastHum = v;
    }
  } // if

  return (newData);
} // _readSensorData()


void DHTElement::_sendSensorData() {
  // dispatch values again.
  LOGGER_ETRACE("resending");
  _dispatch(_tempAction, _lastTemp);
  _dispatch(_humAction, _lastHum);
} // _sendSensorData()


void DHTElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  char tmp[40];
  SensorElement::pushState(callback);
  callback("temperature", _fmt(_lastTemp, tmp));
  callback("humidity", _fmt(_lastHum, tmp));
} // pushState()


// ===== private functions =====

// Formatting a int as decimal number.
char *DHTElement::DHTElement::_fmt(int v, char *s)
{
  // raw format first
  itoa(v, s, 10);
  int l = strlen(s);

  // insert decimal
  if (l > 3) {
    l = l - 2;
    memcpy(s + l + 1, s + l, 3);
    s[l] = '.';
  }
  return (s);
} // _fmt()


// dispatch a single value including formatting as decimal number.
void DHTElement::_dispatch(String &evt, int value)
{
  if (evt.length() > 0) {
    char tmp[10];
    _fmt(value, tmp);
    _board->dispatch(evt, tmp);
  } // if
} // _dispatch()


// End
