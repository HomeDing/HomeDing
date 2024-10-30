/**
 * @file SHT20Element.cpp
 *
 * @brief Element to create actions based on sensor values from a SHT20 Temperature and Humidity sensor.
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
 * Changelog: see SHT20Element.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <sensors/SHT20Element.h>

#include <WireUtils.h>

/**
 * @brief static factory function to create a new SHT20Element
 * @return SHT20Element* created element
 */
Element *SHT20Element::create() {
  return (new SHT20Element());
}  // create()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool SHT20Element::set(const char *name, const char *value) {
  bool ret = true;

  if (SensorElement::set(name, value)) {
    // ok.

  } else if (name == HomeDing::Actions::Address) {
    _address = _atoi(value);
    ret = true;

  } else if (_stricmp(name, "onTemperature") == 0) {
    _actions[0] = value;
    ret = true;

  } else if (name == HomeDing::Actions::OnHumidity) {
    _actions[1] = value;
    ret = true;
  } else {
    ret = false;
  }  // if

  return (ret);
}  // set()

// ===== SHT20 specific commands. see data Data Sheet.

// registers of the chip.

#define SHT20_START_TEMP 0xF3
#define SHT20_START_HUM 0xF5
#define SHT20_RESET 0xFE

//==============================================================================


// calculate crc8 for data
// assuming 0 as a crc start and POLYNOMIAL = 0x131;
// See https://www.sensirion.com/ ... Sensirion_Humidity_Sensors_SHT2x_CRC_Calculation.pdf
uint8_t SHT20Element::_crc8(uint8_t *data, int length) {
  byte crc = 0;

  // calculates 8-Bit checksum with given polynomial

  while (length) {
    crc ^= *data;

    for (int bit = 8; bit > 0; --bit) {
      if (crc & 0x80)
        crc = (crc << 1) ^ 0x131;  // POLYNOMIAL
      else
        crc = (crc << 1);
    }

    length--;
    data++;
  }
  return (crc);
}  // _crc8


uint16_t SHT20Element::_read() {
  uint16_t ret = 0;  // no value
  uint8_t data[3];
  int8_t readLen = 0;

  readLen = WireUtils::readBuffer(_address, data, sizeof(data));
  if ((readLen == sizeof(data)) && (data[2] == SHT20Element::_crc8(data, 2))) {
    // good reading
    ret = ((uint16_t)(data[0]) << 8) + data[1];
  }  // if
  return (ret & 0xFFFC);
}  // _read()


// ===== Element functions

/**
 * @brief Activate the SHT20Element.
 */
void SHT20Element::start() {
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
      _valuesCount = 2;
      _stateKeys = "temperature,humidity";

    }  // if
  }    // if
}  // start()


// ===== Sensor functions

/** return true when reading a probe is done.
 * return any existing value or empty for no data could be read.
 * As the sensor needs 2 cycles of measurements for temp and hum the state is used to control this.
 * 0: start temp
 * 1: read temp
 * 2: start hum
 * 3: read hum
 * 4: report values, reset state
 * 4: report error
 * */
bool SHT20Element::getProbe(String &values) {
  bool newData = false;
  unsigned long now = millis();

  if (_state == 0) {
    _maxTime = now + 100;
    WireUtils::write(_address, SHT20_START_TEMP);
    _state++;

  } else if (_state == 1) {
    uint16_t rawData = _read();
    if (rawData) {
      _temperature = (rawData * (175.72 / 65536)) - 46.85;
      _state++;
    } else if (now > _maxTime) {
      _state = 5;
    }

  } else if (_state == 2) {
    _maxTime = now + 50;
    WireUtils::write(_address, SHT20_START_HUM);
    _state++;

  } else if (_state == 3) {
    uint16_t rawData = _read();
    if (rawData) {
      _humidity = (rawData * ((125.0 / 65536))) - 6.0;
      _state++;
    } else if (now > _maxTime) {
      _state = 5;
    }

  } else if (_state == 4) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%.2f,%.2f", _temperature, _humidity);
    values = buffer;
    newData = true;
    _state = 0;
    // LOGGER_EINFO("values:%s", buffer);

  } else if (_state == 5) {
    values.clear();
    newData = true;
  }
  return (newData);
}  // getProbe()

// End
