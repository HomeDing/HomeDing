/**
 * @file AM2320Element.cpp
 *
 * @brief Element to create actions based on sensor values from a AM2320 Temperature and Humidity sensor.
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
 * Changelog: see AM2320Element.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <sensors/AM2320Element.h>

#include <WireUtils.h>

#define TRACE(...)  // LOGGER_ETRACE(__VA_ARGS__)

/**
 * @brief static factory function to create a new AM2320Element
 * @return AM2320Element* created element
 */
Element *AM2320Element::create() {
  return (new AM2320Element());
}  // create()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool AM2320Element::set(const char *name, const char *value) {
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

// ===== AM2320 specific crc routine.
// calculate crc16 for data (from AM2320 Data Sheet)

uint16_t AM2320Element::_crc16(uint8_t *ptr, int len) {
  uint16_t crc = 0xFFFF;
  int i;

  while (len--) {
    crc ^= *ptr++;
    for (i = 0; i < 8; i++) {
      if (crc & 0x01) {
        crc >>= 1;
        crc ^= 0xA001;
      } else {
        crc >>= 1;
      }
    }  // for
  }    // while
  return (crc);
}

// ===== Element functions

/**
 * @brief Activate the AM2320Element.
 */
void AM2320Element::start() {
  if (!_address) {
    LOGGER_EERR("no address");
    term();

  } else {
    // test if a device is attached
    bool b = WireUtils::exists(_address);
    TRACE("wake1 %d", b);
    if (!b) {
      // try again, as first read is used for wakeup
      b = WireUtils::exists(_address);
      TRACE("wake2 %d", b);
    }

    if (!b) {
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
 * 0: wake up
 * 1: start reading
 * 2: read values
 * */
bool AM2320Element::getProbe(String &values) {
  bool done = false;

  if (_state == 0) {
    // wake()
    WireUtils::exists(_address);
    _state++;

  } else if (_state == 1) {
    // start a reading sequence
    uint8_t buf[3] = { 0x03, 0x00, 0x04 };
    WireUtils::writeBuffer(_address, buf, 3);
    setWait(4); // wait for next communication
    _state = 2;

  } else if (_state == 2) {
    uint8_t outBuf[20];
    WireUtils::readBuffer(_address, outBuf, 4 + 4);

    int16_t crc = (outBuf[7] * 256 + outBuf[6]);
    TRACE(" crc: data=0x%04x calc=0x%04x", crc, _crc16(outBuf, 4 + 2));

    if (crc == _crc16(outBuf, 4 + 2)) {
      // extract temperature
      int v1 = (outBuf[4] & 0x7F) * 256 + outBuf[5];
      int v2 = outBuf[2] * 256 + outBuf[3];

      char buffer[32];
      snprintf(buffer, sizeof(buffer), "%.2f,%.2f", v1 * 0.1, v2 * 0.1);
      values = buffer;

    } else {
      values.clear();  // no values from sensor available
    }
    done = true;
    _state = 0;
  }
  return (done);
}  // getProbe()

// End
