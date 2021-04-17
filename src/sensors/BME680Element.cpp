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
#include <HomeDing.h>

#include <sensors/BME680Element.h>
#include <WireUtils.h>

/* ===== Define local constants and often used strings ===== */

struct bme680_dev gas_sensor;

// ===== adapter functions for bme library

static void delay_msec(uint32_t ms)
{
  delay(ms);
}

static int8_t i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
  int8_t readLen = WireUtils::readBuffer(dev_id, reg_addr, reg_data, len);
  return (readLen != len);
}

static int8_t i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
  WireUtils::write(dev_id, reg_addr, reg_data, len);
  return (0);
}


// ===== private methods

unsigned long BME680Element::beginReading(void)
{
  uint8_t set_required_settings;
  uint16_t meas_period;

  /* Set the temperature, pressure and humidity settings */
  gas_sensor.tph_sett.os_hum = BME680_OS_2X;
  gas_sensor.tph_sett.os_pres = BME680_OS_4X;
  gas_sensor.tph_sett.os_temp = BME680_OS_8X;
  gas_sensor.tph_sett.filter = BME680_FILTER_SIZE_3;

  /* Set the remaining gas sensor settings and link the heating profile */
  gas_sensor.gas_sett.run_gas = BME680_ENABLE_GAS_MEAS;
  /* Create a ramp heat waveform in 3 steps */
  gas_sensor.gas_sett.heatr_temp = 320; /* degree Celsius */
  gas_sensor.gas_sett.heatr_dur = 150; /* milliseconds */

  /* Select the power mode */
  /* Must be set before writing the sensor configuration */
  gas_sensor.power_mode = BME680_FORCED_MODE;

  /* Set the required sensor settings needed */
  set_required_settings = BME680_OST_SEL | BME680_OSP_SEL | BME680_OSH_SEL | BME680_FILTER_SEL | BME680_GAS_SENSOR_SEL;

  /* Set the desired sensor configuration */
  bme680_set_sensor_settings(set_required_settings, &gas_sensor);

  /* Set the power mode */
  bme680_set_sensor_mode(&gas_sensor);

  /* Get the total measurement duration so as to sleep or wait till the measurement is complete */
  bme680_get_profile_dur(&meas_period, &gas_sensor);
  // TRACE("meas_period=%d", meas_period);

  return (millis() + meas_period);
} // beginReading()

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
      _address = _atoi(value);
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
  // TRACE("start()");
  // Setup sensor connectivity and check if sensor is present.
  gas_sensor.dev_id = _address;
  gas_sensor.intf = BME680_I2C_INTF;
  gas_sensor.read = i2c_read;
  gas_sensor.write = i2c_write;
  gas_sensor.delay_ms = delay_msec;
  /* amb_temp can be set to 25 prior to configuring the gas sensor 
     * or by performing a few temperature readings without operating the gas sensor.
     */
  gas_sensor.amb_temp = 25; // just a first guess. will be updated.

  int8_t rslt = bme680_init(&gas_sensor);

  if (rslt != BME680_OK) {
    LOGGER_EERR("no sensor found");
  } else {
    SensorElement::start();
  }
} // start()


bool BME680Element::getProbe(String &values)
{
  bool newData = false;
  char buffer[32];

  if (!_dataAvailable) {
    // start reading
    _dataAvailable = beginReading();

  } else if (_dataAvailable && (millis() < _dataAvailable)) {
    // wait

  } else if (_dataAvailable) {
    // read data from chip
    struct bme680_field_data data;
    int rslt = bme680_get_sensor_data(&data, &gas_sensor);
    if (rslt) {
      LOGGER_EERR("get_sensor_data err %d", rslt);

    } else {
      snprintf(buffer, sizeof(buffer),
               //  "%.2f,%.2f,%.0f,%.0f,%d",
               "%d.%02d,%d.%03d,%d.%02d,%d",
               data.temperature / 100, data.temperature % 100,
               data.humidity / 1000, data.humidity % 1000,
               data.pressure / 100, data.pressure % 100,
               data.gas_resistance); // data.gas_index
      // LOGGER_EINFO("data=%s", buffer);
      // update ambient temperature for next read
      gas_sensor.amb_temp = (data.temperature + 50) / 100;
      // LOGGER_EINFO("amb temp=%d", gas_sensor.amb_temp);
      values = buffer;
      newData = true;
    }
    _dataAvailable = 0;
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
  callback("temperature", Element::getItemValue(_lastValues, 0).c_str());
  callback("humidity", Element::getItemValue(_lastValues, 1).c_str());
  callback("pressure", Element::getItemValue(_lastValues, 2).c_str());
  callback("gas", Element::getItemValue(_lastValues, 3).c_str());
} // pushState()

// End
