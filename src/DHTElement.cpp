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

#include <DHTElement.h>
#include <ElementRegistry.h>
#include <Board.h>

/**
 * @brief static factory function to create a new DHTElement
 * @return DHTElement* created element
 */
Element *DHTElement::create()
{
  return (new DHTElement());
} // create()


DHTElement::DHTElement()
{
  _type = DHTesp::AUTO_DETECT;
  _pin = -1;
  _lastTemp = _lastHum = -666;
  _readTime = 60; // read from sensor once a minute
  _resendTime = 0; // Not enabled resending probes.
} // DHTElement()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool DHTElement::set(const char *name, const char *value)
{
  LOGGER_ETRACE("set(%s:%s)", name, value);
  bool ret = true;

  if (_stricmp(name, "type") == 0) {
    if (_stricmp(value, "DHT11") == 0) {
      _type = DHTesp::DHT11;
    } else if (_stricmp(value, "DHT22") == 0) {
      _type = DHTesp::DHT22;
    } else if (_stricmp(value, "AUTO") == 0) {
      _type = DHTesp::AUTO_DETECT;
    }

  } else if (_stricmp(name, "pin") == 0) {
    _pin = _atopin(value);

  } else if (_stricmp(name, "readtime") == 0) {
    _readTime = _atotime(value);

  } else if (_stricmp(name, "resendtime") == 0) {
    _resendTime = _atotime(value);

  } else if (_stricmp(name, "ontemperature") == 0) {
    _tempAction = value;

  } else if (_stricmp(name, "onhumidity") == 0) {
    _humAction = value;

  } else {
    ret = Element::set(name, value);
  } // if
  return (ret);
} // set()


/**
 * @brief Activate the DHTElement.
 */
void DHTElement::start()
{
  LOGGER_ETRACE("start()");
  unsigned int now = (millis() / 1000);
  if (_pin < 0) {
    LOGGER_EERR("no meaningful pin");

  } else {
    _dht.setup(_pin, _type);
    _lastTemp = _lastHum = -666; // force to send out the values
    _nextRead = now + 2; // now + min. 2 sec., don't hurry
    _nextResend = now + _resendTime;
    Element::start();
  } // if
} // start()


/**
 * @brief check the state of the DHT values and eventually create actions.
 */
void DHTElement::loop()
{
  unsigned int now = millis() / 1000;
  TempAndHumidity values;
  int v;

  if (_nextRead <= now) {
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
        _lastTemp = v;
        _dispatch(_tempAction, _lastTemp);
      } // if

      v = (int)(values.humidity * 100);
      if (v != _lastHum) {
        _lastHum = v;
        _dispatch(_humAction, _lastHum);
      } // if
    } // if
    _nextRead = now + _readTime;

  } else if ((_resendTime > 0) && (_nextResend <= now)) {
    // dispatch values again.
    LOGGER_ETRACE("resending");
    _dispatch(_tempAction, _lastTemp);
    _dispatch(_humAction, _lastHum);
    _nextResend = now + _resendTime;
  } // if

} // loop()


void DHTElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  char tmp[40];
  Element::pushState(callback);
  callback("temperature", _fmt(_lastTemp, tmp));
  callback("humidity", _fmt(_lastHum, tmp));
} // pushState()


// ===== private functions =====

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


void DHTElement::_dispatch(String &evt, int value)
{
  char tmp[10];
  _fmt(value, tmp);
  _board->dispatch(evt, tmp);
} // _dispatch()


// End
