/**
 * @file AHT20Element.cpp
 *
 * @brief Element to create actions based on sensor values from a AHT20 Temperature and Humidity sensor.
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
 * Changelog: see AHT20Element.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <sensors/AHT20Element.h>

#include <WireUtils.h>

#define TRACE(...)  // LOGGER_ETRACE(__VA_ARGS__)

/**
 * @brief static factory function to create a new AHT20Element
 * @return AHT20Element* created element
 */
Element *AHT20Element::create() {
  return (new AHT20Element());
}  // create()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool AHT20Element::set(const char *name, const char *value) {
  bool ret = true;

  if (SensorElement::set(name, value)) {
    // ok.
  } else if (name == HomeDing::Actions::Address) {
    _address = _atoi(value);

  } else if (_stricmp(name, "onTemperature") == 0) {
    _actions[0] = value;

  } else if (_stricmp(name, "onHumidity") == 0) {
    _actions[1] = value;

  } else {
    ret = false;
  }  // if
  return (ret);
}  // set()

//==============================================================================


// calculate crc8 for data
// assuming 0 as a crc start and POLYNOMIAL = 0x131;
// See AHT20 example cope.
uint8_t AHT20Element::_crc8(uint8_t *data, int len) {
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
 * @brief Activate the AHT20Element.
 */
void AHT20Element::start() {
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
      _valuesCount = 2;
      _stateKeys = "temperature,humidity";

      // send initialization sequence
      uint8_t buf[3] = { 0xbe, 0x08, 0x00 };
      WireUtils::writeBuffer(_address, buf, 3);
      setWait(10); // wait at least 10 ms

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
bool AHT20Element::getProbe(String &values) {
  bool newData = false;

  if (_state == 0) {
    // trigger measurement
    uint8_t buf[3] = { 0xac, 0x33, 0x00 };
    WireUtils::writeBuffer(_address, buf, 3);
    setWait(80); // wait at least 80 ms
    _state++;

  } else if (_state == 1) {
    uint8_t data[1];
    WireUtils::readBuffer(_address, data, sizeof(data));

    TRACE("state: %02x", data[0]);
    if (data[0] & 0x80) {
      // wait some more time
      TRACE("wait");
    } else {
      // data is valid
      _state++;
    }

  } else if (_state == 2) {
    uint8_t data[7];
    uint32_t rawData;
    float hum, temp;

    WireUtils::readBuffer(_address, data, sizeof(data));
    TRACE("data: %02x %02x %02x %02x %02x - %02x", data[1], data[2], data[3], data[4], data[5], data[6]);

    uint8_t c = AHT20Element::_crc8(data, 6);
    TRACE("crc: %02x", c);

    if (c == data[6]) {
      rawData = data[1] << 8;
      rawData = (rawData | data[2]) << 8;
      rawData = (rawData | data[3]);
      rawData = rawData >> 4;
      hum = ((float)rawData * 100.0) / 0x100000;

      rawData = data[3] << 8;
      rawData = (rawData | data[4]) << 8;
      rawData = (rawData | data[5]);
      rawData = rawData & 0xfffff;
      temp = ((float)rawData * 200.0 / 0x100000) - 50;

      char buffer[32];
      snprintf(buffer, sizeof(buffer), "%.2f,%.2f", temp, hum);
      TRACE("values: %s", buffer);
      values = buffer;
    }  // if

    newData = true;
    _state = 0;
  }
  return (newData);
}  // getProbe()

// End
