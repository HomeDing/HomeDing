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
#include <Element.h>
#include <Board.h>

#include <sensors/BME680Element.h>
#include <ElementRegistry.h>

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

// BME680Element::BME680Element() {}

// void BME680Element::init(Board *board)
// {
//   Element::init(board);
//   // do something here like initialization
// } // init()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool BME680Element::set(const char *name, const char *value)
{
  bool ret = true;

  if (_stricmp(name, "readtime") == 0) {
    _readTime = _atotime(value);

  } else if (_stricmp(name, "address") == 0) {
    _address = strtol(value, nullptr, 0);

  } else if (_stricmp(name, "ontemperature") == 0) {
    _temperatureAction = value;

  } else if (_stricmp(name, "onhumidity") == 0) {
    _humidityAction = value;

  } else if (_stricmp(name, "onpressure") == 0) {
    _pressureAction = value;

  } else if (_stricmp(name, "ongas") == 0) {
    _gasAction = value;

  } else {
    ret = Element::set(name, value);
  } // if

  return (ret);
} // set()


/**
 * @brief Activate the BME680Element.
 */
void BME680Element::start()
{
  LOGGER_ETRACE("start()");

  if (!_bme.begin(_address)) {
    LOGGER_EERR("no BME680 sensor found");
  } else {
    // Set up oversampling and filter initialization
    _bme.setTemperatureOversampling(BME680_OS_8X);
    _bme.setHumidityOversampling(BME680_OS_2X);
    _bme.setPressureOversampling(BME680_OS_4X);
    _bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
    _bme.setGasHeater(320, 150); // 320*C for 150 ms

    _state = 0; // no reading initiated
    _nextRead = _board->getSeconds() + 2;

    Element::start();
  } // if

} // start()


// handle a new float value.
void BME680Element::_newValue(String &strVal, const char *fmt, float value,
                              String &actions)
{
  char tmp[12];
  snprintf(tmp, sizeof(tmp), fmt, value);
  if (!strVal.equals(tmp)) {
    // LOGGER_EINFO("new %s", tmp);
    strVal = tmp;
    // send actions
    _board->dispatch(actions, tmp);
  }
} // _newValue()


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void BME680Element::loop()
{
  unsigned int now = _board->getSeconds();
  char tmp[12];

  if ((_state == 0) && (_nextRead < now)) {
    // initiate a reading
    _endTime = _bme.beginReading();
    _state = 1; // reading initiated, wait for _endTime

  } else if ((_state == 1) && (_endTime < millis())) {
    _bme.endReading();

    _newValue(_temperature, "%.2f", _bme.temperature, _temperatureAction);
    _newValue(_humidity, "%.2f", _bme.humidity, _humidityAction);
    _newValue(_pressure, "%.0f", _bme.pressure, _pressureAction);
    _newValue(_gas, "%.0f", _bme.gas_resistance, _gasAction);

    _nextRead = _board->getSeconds() + _readTime;
    _state = 0; // wait for next time

  } // if
} // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void BME680Element::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
  callback("temperature", _temperature.c_str());
  callback("humidity", _humidity.c_str());
  callback("pressure", _pressure.c_str());
  callback("gas", _gas.c_str());
} // pushState()

// maybe: overwrite the term() function,

// End
