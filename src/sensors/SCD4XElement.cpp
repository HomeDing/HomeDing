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
  bool ret = true;

  if (SensorElement::set(name, value)) {
    // ok.
  } else if (_stricmp(name, "address") == 0) {
    _address = _atoi(value);

  } else if (_stricmp(name, "onCO2") == 0) {
    _actions[0] = value;

  } else if (_stricmp(name, "onTemperature") == 0) {
    _actions[1] = value;

  } else if (_stricmp(name, "onHumidity") == 0) {
    _actions[2] = value;

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
  if (!_address) {
    LOGGER_EERR("no address");
    term();

  } else {
    // test if a device is attached
    if (!WireUtils::exists(_address)) {
      LOGGER_EERR("not found");
      term();

    } else {
      TRACE("init");
      SensorElement::start();

      // send initialization sequence, in periodic mode measurement
      uint8_t buf[2] = { 0x21, 0xb1 };
      WireUtils::writeBuffer(_address, buf, 2);
      setWait(5000);  // wait at least 5 seconds

    }  // if
  }    // if
}  // start()


// ===== Sensor functions

/** return true when reading a probe is done.
 * return any existing value or empty for no data could be read.
 * As the sensor needs 2 cycles of measurements for temp and hum the state is used to control this.
 * 0: trigger measurement
 * 1: read status
 * 2: start data
 * */
bool SCD4XElement::getProbe(String &values) {
  TRACE("getProbe()");
  bool newData = false;
  unsigned long now = millis();

  if (_state == 0) {
    // send read data command
    uint8_t buf[2] = { 0xec, 0x05 };
    WireUtils::writeBuffer(_address, buf, sizeof(buf));
    _state = 1;

  } else if (_state == 1) {
    uint8_t data[12];
    uint8_t readLen = WireUtils::readBuffer(_address, data, sizeof(data));
    TRACE("len: %d", readLen);

    if (readLen != sizeof(data)) {
      setWait(1000);  // wait another time to get a valid measurement.

    } else {
      // check all crc data, ignore data with wrong crc
      if ((_crc8(data + 0, 2) == data[2]) && (_crc8(data + 3, 2) == data[5]) && (_crc8(data + 6, 2) == data[8])) {
        // see data conversions from the datasheet
        float co2 = (float)((uint16_t)data[0] << 8 | data[1]);
        float temp = -45 + 175 * (float)((uint16_t)data[3] << 8 | data[4]) / 65536;
        float hum = 100 * (float)((uint16_t)data[6] << 8 | data[7]) / 65536;

        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%.0f,%.2f,%.2f", co2, temp, hum);
        values = buffer;
        TRACE("values: %s", buffer);
      }
    }
    newData = true;
    _state = 0;
  }
  return (newData);
}  // getProbe()

// End
