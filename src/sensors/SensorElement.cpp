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

/**
 * @brief static factory function to create a new SensorElement
 * @return SensorElement* created element
 */
Element *SensorElement::create()
{
  return (new SensorElement());
} // create()


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
  unsigned int now = _board->getSeconds();
  Element::start();

  _nextRead = now + 2; // now + min. 2 sec., don't hurry
  _nextResend = now + _resendTime;
} // start()


/**
 * @brief check the state of the DHT values and eventually create actions.
 */
void SensorElement::loop()
{
  unsigned int now = _board->getSeconds();
  bool newData = false;

  if (_nextRead <= now) {
    // LOGGER_ETRACE("start reading...");
    newData = _readSensorData();
  } // if

  if ((newData) || ((_resendTime > 0) && (_nextResend <= now))) {
    // LOGGER_ETRACE("start sending...");
    _sendSensorData();
    _nextRead = now + _readTime;
    _nextResend = now + _resendTime;
  } // if
} // loop()


void SensorElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
} // pushState()


// ===== private functions =====

bool SensorElement::_readSensorData()
{
  // no read
  return (false);
};

void SensorElement::_sendSensorData(){
    // no send
};


// End
