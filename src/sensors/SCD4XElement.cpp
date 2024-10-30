/**
 * @file SCD4XElement.cpp
 *
 * @brief Element to create actions based on sensor values from a SCD4X Temperature and Humidity sensor.
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
 * Changelog: see SCD4XElement.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <sensors/SCD4XElement.h>
#include <ListUtils.h>

#include <WireUtils.h>

#define TRACE(...)  // LOGGER_ETRACE(__VA_ARGS__)

/**
 * @brief static factory function to create a new SCD4XElement
 * @return SCD4XElement* created element
 */
Element *SCD4XElement::create() {
  return (new SCD4XElement());
}  // create()


SCD4XElement::SCD4XElement() {
  // initialize for SCD4X
  _valuesCount = 3;
  _stateKeys = "co2,temperature,humidity";
}

/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool SCD4XElement::set(const char *name, const char *value) {
  // TRACE("set %s=%s", name, value);
  bool ret = true;

  if (SensorElement::set(name, value)) {
    // ok.
  } else if (name == HomeDing::Actions::Address) {
    _address = _atoi(value);

  } else if (_stricmp(name, "mode") == 0) {
    SCANMODE m = (SCANMODE)ListUtils::indexOf("continuous,lowpower,single", value);
    if ((m >= SCANMODE::_min) && (m <= SCANMODE::_max))
      _mode = m;

  } else if (_stricmp(name, "onCO2") == 0) {
    _actions[0] = value;

  } else if (_stricmp(name, "onTemperature") == 0) {
    _actions[1] = value;

  } else if (_stricmp(name, "onHumidity") == 0) {
    _actions[2] = value;

  } else if ((active) && (_stricmp(name, "factoryReset") == 0)) {
    uint8_t buf[2] = { 0x36, 0x32 };  // factory-init sequence
    WireUtils::writeBuffer(_address, buf, 2);
    setWait(1200);  // wait at least 20 milli seconds

  } else {
    ret = false;
  }  // if
  return (ret);
}  // set()

//==============================================================================


// calculate crc8 for data
// assuming 0 as a crc start and POLYNOMIAL = 0x131;
// See SCD4X example cope.
uint8_t SCD4XElement::_crc8(uint8_t *data, int len) {
  uint8_t i;
  uint8_t byte;
  uint8_t crc = 0xFF;

  for (byte = 0; byte < len; byte++) {
    crc ^= (data[byte]);
    for (i = 8; i > 0; --i) {
      if (crc & 0x80) crc = (crc << 1) ^ 0x31;
      else
        crc = (crc << 1);
    }
  }
  return (crc);
}  // _crc8


// ===== Element functions

/**
 * @brief Activate the SCD4XElement.
 */
void SCD4XElement::start() {
  TRACE("start()");
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

      // send re-init sequence
      uint8_t buf[2] = { 0x36, 0x46 };
      WireUtils::writeBuffer(_address, buf, 2);
      setWait(20);  // wait at least 20 milli seconds
      _state = 0;

    }  // if
  }    // if
}  // start()


// ===== Sensor functions

/** Return true when reading a probe is done.
 * Return any existing value or empty for no data could be read.
 * 0: start meassurement
 * 1: start read
 * 2: read
 */

bool SCD4XElement::getProbe(String &values) {
  TRACE("getProbe(%d)", _state);
  bool newData = false;
  unsigned long now = millis();
  uint8_t buf[2];
  int waitTime = 0;

  if (_state == 0) {
    // Send a start command...
    TRACE("start Mode(%d)", _mode);
    buf[0] = 0x21;
    if (_mode == SCANMODE::CONTINUOUS) {
      buf[1] = 0xb1;  // start periodic measurement
      waitTime = 5000;
    } else if (_mode == SCANMODE::LOWPOWER) {
      buf[1] = 0xac;  // start periodic low power measurement
      waitTime = 30000;
    } else if (_mode == SCANMODE::SINGLE) {
      buf[1] = 0x9d;  // start single measurement mode
      waitTime = 5000;
    }
    setWait(waitTime);
    WireUtils::writeBuffer(_address, buf, 2);
    _state = 1;

  } else if (_state == 1) {
    // read data ready
    uint8_t buf[2] = { 0xe4, 0xb8 };
    uint8_t data[3];

    WireUtils::writeBuffer(_address, buf, 2);
    WireUtils::readBuffer(_address, data, sizeof(data));

    TRACE("state: %02x %02x", data[0], data[1]);
    if ((data[0] << 8 | data[1]) & 0x07FF) {
      // data ready
      _state = 2;

    } else if (now - _startTime < 3000) {
      setWait(500);  // wait some time for data ...

    } else {
      // re-init
      _state = 0;
    };

  } else if (_state == 2) {
    // send read data command

    uint8_t buf[2] = { 0xec, 0x05 };
    WireUtils::writeBuffer(_address, buf, sizeof(buf));

    uint8_t data[12];
    uint8_t readLen = WireUtils::readBuffer(_address, data, sizeof(data));
    TRACE("len: %d", readLen);

    if (readLen != sizeof(data)) {
      LOGGER_EERR("bad size");
      term();

    } else {
      // check all crc data, ignore data with wrong crc
      if ((_crc8(data + 0, 2) == data[2]) && (_crc8(data + 3, 2) == data[5]) && (_crc8(data + 6, 2) == data[8])) {
        // see data conversions from the datasheet
        float co2 = (float)((uint16_t)data[0] << 8 | data[1]);
        float temp = -45 + 175 * (float)((uint16_t)data[3] << 8 | data[4]) / 65536;
        float hum = 100 * (float)((uint16_t)data[6] << 8 | data[7]) / 65536;

        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%.0f, %.2f, %.2f", co2, temp, hum);
        values = buffer;
        TRACE("values: %s", buffer);

        if (_mode != SCANMODE::SINGLE) {
          _state = 1;
        } else {
          _state = 0;
        }
      } else {
        LOGGER_EERR("bad checksum");
        term();
      }
    }
    newData = true;
  }
  return (newData);
}  // getProbe()


// End
