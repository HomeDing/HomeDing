/**
 * @file BH1750Element.cpp
 *
 * @brief Element to create actions based on sensor values from a BH1750 Light sensor.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD 3-Clause style license,
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino.
 *
 * Changelog: see BH1750Element.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include "BH1750Element.h"

#include <WireUtils.h>

#define TRACE(...) LOGGER_JUSTINFO(__VA_ARGS__)

/**
 * @brief static factory function to create a new BH1750Element
 * @return BH1750Element* created element
 */
Element *BH1750Element::create() {
  return (new BH1750Element());
}  // create()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool BH1750Element::set(const char *name, const char *value) {
  TRACE("set %s=%s", name, value);
  bool ret = true;

  if (SensorElement::set(name, value)) {
    // ok.

  } else if (_stricmp(name, "address") == 0) {
    _address = _atoi(value);

  } else if (_stricmp(name, "onvalue") == 0) {
    _valueAction = value;

  } else if (_stricmp(name, "mode") == 0) {
    int m = _atoi(value);
    if ((m == 0) || (m == 1) || (m == 3))
      _mode = 0x20 + m;

  } else if (_stricmp(name, "factor") == 0) {
    _factor = atof(value);

  } else {
    ret = false;
  }  // if
  return (ret);
}  // set()


// ===== BH1750 specific commands. see data Data Sheet.

// codes for operation mode

// Mode         | code | resolution | Measurement Time
// Continuously | 0x10 | 1lx        | 120ms
// Continuously | 0x11 | 0.5lx      | 120ms
// Continuously | 0x13 | 4lx        | 16ms
// One Time     | 0x20 | 1lx        | 120ms
// One Time     | 0x21 | 0.5lx      | 120ms
// One Time     | 0x23 | 4lx        | 16ms

//==============================================================================


// ===== Element functions

/**
 * @brief Activate the BH1750Element.
 */
void BH1750Element::start() {
  if (!_address) {
    LOGGER_EERR("no address");
    term();

  } else {
    // test if a device is attached
    if (!WireUtils::exists(_address)) {
      LOGGER_EERR("not found");
      term();

    } else {
      SensorElement::start();
    }  // if
  }    // if
}  // start()


// ===== Sensor functions

/** return true when reading a probe is done.
 * return any existing value or empty for no data could be read.
 * */
bool BH1750Element::getProbe(String &values) {
  // TRACE("getProbe()");
  bool newData = false;
  unsigned long now = millis();

  if (_dataIsReady == 0) {
    TRACE("startProbe");

    WireUtils::write(_address, _mode, nullptr, 0);  // One Time H-Resolution Mode
    _dataIsReady = now + ((_mode == 0x23) ? 20 : 130);

  } else if (_dataIsReady < now) {
    uint8_t data[4];
    WireUtils::request(_address, data, 2);  // One Time H-Resolution Mode

    unsigned int count = (data[0] << 8) + data[1];

    TRACE("got: %d %d %d", data[0], data[1], val);

    values = String(_factor * count, 1);
    newData = true;
    _dataIsReady = 0;
  }

  return (newData);
}  // getProbe()


void BH1750Element::sendData(String &values) {
  // dispatch values.
  _board->dispatchItem(_valueAction, values, 0);
}  // sendData()


void BH1750Element::pushState(
  std::function<void(const char *pName, const char *eValue)> callback) {
  SensorElement::pushState(callback);
  callback("value", _lastValues.c_str());
}  // pushState()

bool BH1750Element::registered =
  ElementRegistry::registerElement("bh1750", BH1750Element::create);

// End
