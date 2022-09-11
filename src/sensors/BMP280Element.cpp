/**
 * @file BMP280Element.cpp
 *
 * @brief Optional Input Element for the HomeDing Library to read DS18B20 sensors and create actions.
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
 * Changelog: see BMP280Element.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <sensors/BMP280Element.h>

#include <WireUtils.h>

/**
 * @brief static factory function to create a new BMP280Element
 * @return BMP280Element* created element
 */
Element *BMP280Element::create() {
  return (new BMP280Element());
}  // create()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool BMP280Element::set(const char *name, const char *value) {
  bool ret = true;

  if (SensorElement::set(name, value)) {
    // ok.
  } else if (_stricmp(name, "address") == 0) {
    _address = _atoi(value);

  } else if (_stricmp(name, "onTemperature") == 0) {
    _actions[0] = value;

  } else if (_stricmp(name, "onPressure") == 0) {
    _actions[1] = value;

  } else {
    ret = false;
  }  // if
  return (ret);
}  // set()

// ===== BMP280 specific funtions. see data Data Sheet.

// registers of the chip.

#define BMP280_REG_CALIB 0x88  // 24 bytes for callibration
#define BMP280_REG_ID 0xD0
#define BMP280_REG_RESET 0xE0
#define BMP280_REG_STATE 0xF3
#define BMP280_REG_CONTROL 0xF4
#define BMP280_REG_CONFIG 0xF5
#define BMP280_REG_DATA 0xF7  // 6 bytes of data

// Control settings
#define BMP280_MODE_SLEEP 0x00
#define BMP280_MODE_FORCED 0x01
#define BMP280_MODE_NORMAL 0x03
#define BMP280_OSP_1 (0x01 << 2)
#define BMP280_OSP_4 (0x03 << 2)
#define BMP280_OSP_16 (0x05 << 2)
#define BMP280_OST_1 (0x01 << 5)
#define BMP280_OST_4 (0x03 << 5)
#define BMP280_OST_16 (0x05 << 5)

#define FORCED

// use T1-T3 to calculate right, compensated temperature
// Returns temperature in Deg C, resolution is 0.01.
float BMP280Element::BMP280CompensateTemperature(int32_t adc_T) {
  int32_t T;

  int32_t var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
  int32_t var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
  t_fine = var1 + var2;  // required in BMP280_compensate_Pressure
  T = (t_fine * 5 + 128) >> 8;
  return ((float)T / 100);
}  // BMP280CompensateTemperature


// Use P1-P9 to calculate compensated pressure.
// Returns pressure in Pa.
float BMP280Element::BMP280CompensatePressure(int32_t adc_P) {
  int64_t var1, var2, p;

  adc_P = 398568L;

  var1 = ((int64_t)t_fine) - 128000;
  var2 = var1 * var1 * (int64_t)dig_P6;
  // TRACE("v2:%" PRId64, var2);
  var2 = var2 + ((var1 * (int64_t)dig_P5) << 17);
  var2 = var2 + (((int64_t)dig_P4) << 35);
  var1 = ((var1 * var1 * (int64_t)dig_P3) >> 8) + ((var1 * (int64_t)dig_P2) << 12);
  var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)dig_P1) >> 33;

  if (var1 == 0) {
    return 0;  // avoid exception caused by division by zero
  }
  p = 1048576 - adc_P;
  p = (((p << 31) - var2) * 3125) / var1;
  var1 = (((int64_t)dig_P9) * (p >> 13) * (p >> 13)) >> 25;
  var2 = (((int64_t)dig_P8) * p) >> 19;
  p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7) << 4);
  return ((float)p / 256);
}  // BMP280CompensatePressure()

// float BMP280::calcAltitude(float pressure)
// {
//   float A = pressure / 101325;
//   float B = 1 / 5.25588;
//   float C = pow(A, B);
//   C = 1.0 - C;
//   C = C / 0.0000225577;
//   return C;
// }

// float getAltitude(float press, float temp) {
//  return ((pow((sea_press / press), 1/5.257) - 1.0) * (temp + 273.15)) / 0.0065;
// }


// ===== Element functions

/**
 * @brief Activate the BMP280Element.
 */
void BMP280Element::start() {
  if (!_address) {
    LOGGER_EERR("no address");
    term();

  } else {
    // test if a device is attached
    if (!WireUtils::exists(_address)) {
      LOGGER_EERR("not found");
      term();

    } else {
      _valuesCount = 2;
      _stateKeys = "temperature,pressure";
      SensorElement::start();

#if defined(FORCED)
      // initialize chip: forced, max oversampling
      WireUtils::write(_address, BMP280_REG_CONFIG, 0);
      WireUtils::write(_address, BMP280_REG_CONTROL, BMP280_MODE_SLEEP | BMP280_OSP_16 | BMP280_OST_16);
#else
      // initialize chip: 1 sec sampling, max oversampling
      WireUtils::write(_address, BMP280_REG_CONFIG, 0x05 << 5);
      WireUtils::write(_address, BMP280_REG_CONTROL, BMP280_MODE_NORMAL | BMP280_OSP_16 | BMP280_OST_16);
#endif

      // read all raw calibration data, see table 17
      uint8_t data[24];
      WireUtils::readBuffer(_address, BMP280_REG_CALIB, data, 24);

      dig_T1 = WU_U16(data, 0);
      dig_T2 = WU_S16(data, 2);
      dig_T3 = WU_S16(data, 4);
      // LOGGER_INFO("calib: %d %d %d ", dig_T1, dig_T2, dig_T3);

      dig_P1 = WU_U16(data, 6);
      dig_P2 = WU_S16(data, 8);
      dig_P3 = WU_S16(data, 10);
      dig_P4 = WU_S16(data, 12);
      dig_P5 = WU_S16(data, 14);
      dig_P6 = WU_S16(data, 16);
      dig_P7 = WU_S16(data, 19);
      dig_P8 = WU_S16(data, 20);
      dig_P9 = WU_S16(data, 22);
      // LOGGER_INFO("calib: %d %d %d %d %d %d %d %d %d ", dig_P1, dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9);
    }  // if
  }    // if
}  // start()


// ===== Sensor functions

/** return true when reading a probe is done.
 * return any existing value or empty for no data could be read. */
bool BMP280Element::getProbe(String &values) {
  // TRACE("getProbe()");
  bool newData = false;

#if defined(FORCED)
  if (_state == 0) {
    // start new sampling
    // TRACE("start sampling...");
    WireUtils::write(_address, BMP280_REG_CONTROL, BMP280_MODE_FORCED | BMP280_OSP_16 | BMP280_OST_16);
    _state = 1;
    return (newData);  // no data available now
  }                    // if

  // WireUtils::dump(_address, BMP280_REG_CONFIG, 1);

  uint8_t busy = WireUtils::readRegister(_address, BMP280_REG_CONFIG);
  if (busy != 0) {
    // TRACE("wait...");
    return (newData);  // not ready yet
  }
  _state = 0;  // can read now
#endif

  // read out data
  uint8_t data[6];
  WireUtils::readBuffer(_address, BMP280_REG_DATA, data, 6);
  // TRACE("data %02x %02x %02x %02x %02x %02x", data[0], data[1], data[2], data[3], data[4], data[5]);

  int32_t adc_T = data[3] << 12 | data[4] << 4 | data[5] >> 4;
  float T = BMP280CompensateTemperature(adc_T);
  // TRACE("raw_T %d => %.2f", adc_T, T);

  int32_t adc_P = data[0] << 12 | data[1] << 4 | data[2] >> 4;
  float P = BMP280CompensatePressure(adc_P);
  // TRACE("raw_P %d => %.2f", adc_P, P);

  char buffer[32];
  snprintf(buffer, sizeof(buffer), "%.2f,%.0f", T, P);
  values = buffer;

  newData = true;

  return (newData);
}  // getProbe()


// End
