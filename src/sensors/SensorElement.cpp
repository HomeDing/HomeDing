/**
 * @file SensorElement.cpp
 *
 * @brief This Input Element acts as the base class for some sensor elements that need collecting sensor data on a regular basis.
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
 * Changelog: see SensorElement.h
 */

#include <Arduino.h>
#include <Board.h>
#include <Element.h>

#include <sensors/SensorElement.h>


SensorElement::SensorElement()
{
  _readTime = 60; // read from sensor once a minute
  _resendTime = 0; // Not enabled resending probes.
} // SensorElement()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool SensorElement::set(const char *name, const char *value)
{
  bool ret = Element::set(name, value);

  if (!ret) {
    if (_stricmp(name, "readtime") == 0) {
      _readTime = _atotime(value);
      ret = true;
    } else if (_stricmp(name, "resendtime") == 0) {
      _resendTime = _atotime(value);
      ret = true;
    } // if
  } // if
  return (ret);
} // set()


/**
 * @brief Activate the SensorElement.
 */
void SensorElement::start()
{
  LOGGER_ETRACE("start()");
  unsigned int now = millis();
  Element::start();

  _nextRead = now + 2 * 1000; // now + min. 2 sec., don't hurry
  _nextSend = 0;
} // start()


/**
 * @brief check the state of the DHT values and eventually create actions.
 */
void SensorElement::loop()
{
  unsigned long now = millis();
  bool newData = false;

  if (_nextRead <= now) {
    // time to get sensor data
    // LOGGER_ETRACE("start reading...");
    bool done = getProbe(_values);
    if (done) {
      _nextSend = now;
      _nextRead = now + _readTime * 1000;
    } // if
  } // if

  if (_nextSend && (_nextSend < now)) {
    // LOGGER_ETRACE("start sending... %d", _resendTime);
    sendData(_values);
    _nextSend = _resendTime ? now + _resendTime * 1000 : 0;
  } // if
} // loop()


void SensorElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
} // pushState()


// ===== private functions =====

bool SensorElement::getProbe(String &values)
{

  values.clear(); // no values
  return (true); // always over
};


void SensorElement::sendData(String &values)
{
};


// End
