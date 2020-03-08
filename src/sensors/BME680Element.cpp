/**
 * @file BME680Element.cpp
 * @brief Element for reading BME680 sensor data.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license.
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:see BME680Element.h
 */

#include <Arduino.h>
#include <Board.h>
#include <Element.h>

#include <ElementRegistry.h>
#include <sensors/BME680Element.h>

/* ===== Define local constants and often used strings ===== */

// #define SEALEVELPRESSURE_HPA (1013.25)


/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new BME680Element
 * @return BME680Element* created element
 */
Element *BME680Element::create()
{
  return (new BME680Element());
} // create()


/* ===== Element functions ===== */


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool BME680Element::set(const char *name, const char *value)
{
  bool ret = SensorElement::set(name, value);

  if (!ret) {
    if (_stricmp(name, PROP_ADDRESS) == 0) {
      _address = atoi(value);
      ret = true;

    } else if (_stricmp(name, ACTION_ONTEMPERATURE) == 0) {
      _temperatureAction = value;
      ret = true;

    } else if (_stricmp(name, ACTION_ONHUMIDITY) == 0) {
      _humidityAction = value;
      ret = true;

    } else if (_stricmp(name, ACTION_ONPRESSURE) == 0) {
      _pressureAction = value;
      ret = true;

    } else if (_stricmp(name, "ongas") == 0) {
      _gasAction = value;
      ret = true;
    } // if
  } // if

  return (ret);
} // set()


/**
 * @brief Activate the BME680Element.
 */
void BME680Element::start()
{
  // LOGGER_ETRACE("start()");

  if (!_bme.begin(_address)) {
    LOGGER_EERR("no sensor found");
  } else {
    // Set up oversampling and filter initialization
    _bme.setTemperatureOversampling(BME680_OS_8X);
    _bme.setHumidityOversampling(BME680_OS_2X);
    _bme.setPressureOversampling(BME680_OS_4X);
    _bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
    _bme.setGasHeater(320, 150); // 320*C for 150 ms

    SensorElement::start();
  } // if

} // start()


// read from float value and check if changed.
bool BME680Element::_readValue(String &strVal, const char *fmt, float value)
{
  bool ret = false;
  char tmp[12];
  snprintf(tmp, sizeof(tmp), fmt, value);
  if (!strVal.equals(tmp)) {
    ret = true;
    strVal = tmp;
  }
} // _readValue()


bool BME680Element::getProbe(String &values)
{
  bool newData = false;
  char buffer[32];

  if (!_dataAvailable) {
    // start reading
    _dataAvailable = _bme.beginReading();

  } else if (_dataAvailable && (millis() < _dataAvailable)) {
    // wait

  } else if (_dataAvailable) {
    _bme.endReading();
    snprintf(buffer, sizeof(buffer),
             "%.2f,%.2f,%.0f,%.0f",
             _bme.temperature, _bme.humidity, _bme.pressure, _bme.gas_resistance);
    values = buffer;
    _dataAvailable = 0;
    newData = true;
  }
  return (newData);
} // getProbe()


void BME680Element::sendData(String &values)
{
  _board->dispatchItem(_temperatureAction, values, 0);
  _board->dispatchItem(_humidityAction, values, 1);
  _board->dispatchItem(_pressureAction, values, 2);
  _board->dispatchItem(_gasAction, values, 3);
} // sendData()


/**
 * @brief push the current value of all properties to the callback.
 */
void BME680Element::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  SensorElement::pushState(callback);
  callback("temperature", Element::Element::getItemValue(_lastValues, 0).c_str());
  callback("humidity", Element::Element::getItemValue(_lastValues, 1).c_str());
  callback("pressure", Element::Element::getItemValue(_lastValues, 2).c_str());
  callback("gas", Element::Element::getItemValue(_lastValues, 3).c_str());
} // pushState()

// End
