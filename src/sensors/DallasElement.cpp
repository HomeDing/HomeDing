/**
 * @file DallasElement.cpp
 *
 * @brief Sensor Element for the HomeDing Library to read DS18B20 and other OneLine sensors supported
 * by the DallasTemperature library and create actions.
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
 * Changelog: see DallasElement.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <sensors/DallasElement.h>

#define TRACE(...)  // LOGGER_JUSTINFO(__VA_ARGS__)

#include <OneWire.h>
#include <DallasTemperature.h>

class DallasElementImpl {
public:
  OneWire *oneWire;
  DallasTemperature *sensors;

  int pin = -1;
  uint8_t addr[8];
  uint8_t resolution;

  /** time in msecs when data is ready */
  unsigned long dataIsReady;

  /**
   * @brief The tempAction is emitted when a new temp was read from the DHT
   * sensor.
   */
  String tempAction;
};

/**
 * @brief static factory function to create a new DallasElement
 * @return DallasElement* created element
 */
Element *DallasElement::create() {
  return (new DallasElement());
}  // create()


/**
 * @brief Construct a new DallasElement
 */
DallasElement::DallasElement() {
  _impl = new DallasElementImpl();
  pinMode(12, OUTPUT);
}


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool DallasElement::set(const char *name, const char *value) {
  bool ret = true;

  if (SensorElement::set(name, value)) {
    // done.

  } else if (name == HomeDing::Actions::Pin) {
    _impl->pin = _atopin(value);

  } else if (_stricmp(name, "resolution") == 0) {
    int r = atoi(value);
    _impl->resolution = constrain(r, 9, 12);

  } else if (_stricmp(name, HomeDing::Actions::OnTemperature) == 0) {
    _impl->tempAction = value;
  } else {
    ret = false;
  }  // if
  return (ret);
}  // set()


/**
 * @brief Activate the DallasElement.
 */
void DallasElement::start() {
  TRACE("start(pin=%d)", _impl->pin);
  int n = 0;
  if (_impl->pin >= 0) {

    _impl->oneWire = new (std::nothrow) OneWire(_impl->pin);

    if (_impl->oneWire) {
      _impl->sensors = new (std::nothrow) DallasTemperature(_impl->oneWire);

      _impl->sensors->begin();
      n = _impl->sensors->getDeviceCount();
      TRACE("count=%d", n);
    }

    if ((_impl->oneWire) && (n > 0)) {
      // _impl->sensors->getAddress(_impl->addr, 0);
      // uint8_t *a = _impl->addr;
      // TRACE("Address: %02x %02x %02x %02x %02x %02x %02x %02x", a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]);
      _impl->sensors->setWaitForConversion(false);
      _impl->sensors->setResolution(_impl->resolution);
      SensorElement::start();
    } else {
      LOGGER_EERR("no address");
    }
  }  // if
}  // start()


/** return true when reading a probe is done.
 * return any existing value or empty for no data could be read. */
bool DallasElement::getProbe(String &values) {
  // TRACE("getProbe()");
  bool newData = false;
  unsigned long now = millis();

  if (_impl->dataIsReady == 0) {
    // start conversion now
    TRACE("probe-start");
    _impl->sensors->requestTemperatures();
    _impl->dataIsReady = now + _impl->sensors->millisToWaitForConversion(_impl->resolution);

  } else if (_impl->dataIsReady < now) {
    TRACE("probe-done");
    float temp = _impl->sensors->getTempCByIndex(0);
    values = String(temp, 2);

    _impl->dataIsReady = 0;
    newData = true;
  }  // if

  return (newData);
}  // getProbe()


void DallasElement::sendData(String &values) {
  // dispatch value.
  TRACE("sending %s", values.c_str());
  HomeDing::Actions::push(_impl->tempAction, values);
}  // sendData()


void DallasElement::pushState(
  std::function<void(const char *pName, const char *eValue)> callback) {
  SensorElement::pushState(callback);
  callback("temperature", _lastValues.c_str());
}  // pushState()


// ===== private functions =====

// End
