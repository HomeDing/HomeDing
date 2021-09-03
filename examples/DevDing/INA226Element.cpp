/**
 * @file INA226Element.cpp
 * @brief Element to control a INA219 power consumption chip.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD 3-Clause style license,
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:see INA226Element.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include "INA226Element.h"


#define TRACE(...) // LOGGER_ETRACE(__VA_ARGS__)

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new INA226Element
 * @return INA226Element* created element
 */
Element *INA226Element::create() {
  return (new INA226Element());
} // create()


/* ===== Element functions ===== */

INA226Element::INA226Element()
    : _address(0x40),
      _sensor(nullptr),
      _mode(INA226_MEASURE_MODE::CONTINUOUS),
      _shunt(0.002),
      _cRange(16),
      _convTime(INA226_CONV_TIME::CONV_TIME_8244),
      _averageMode(INA226_AVERAGES::AVERAGE_16) {}


void INA226Element::init(Board *board) {
  TRACE("init()");
  SensorElement::init(board);
} // init()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool INA226Element::set(const char *name, const char *value) {
  bool ret = SensorElement::set(name, value);

  if (!ret) {
    // Not handled by the SensorElement
    ret = true; // assume
    if (_stricmp(name, "onVoltage") == 0) {
      _voltageAction = value;

    } else if (_stricmp(name, "onCurrent") == 0) {
      _currentAction = value;

    } else if (_stricmp(name, "onPower") == 0) {
      _powerAction = value;

    } else if (_stricmp(name, "mode") == 0) {
      if (_stricmp(value, "once") == 0)
        _mode = INA226_MEASURE_MODE::TRIGGERED;
      else if (_stricmp(value, "continuous") == 0)
        _mode = INA226_MEASURE_MODE::CONTINUOUS;

    } else if (_stricmp(name, "shunt") == 0) {
      _shunt = atof(value);

    } else if (_stricmp(name, "range") == 0) {
      _cRange = atof(value);

    } else if (_stricmp(name, "convTime") == 0) {
      if (_stricmp(value, "140") == 0)
        _convTime = INA226_CONV_TIME::CONV_TIME_140;
      else if (_stricmp(value, "204") == 0)
        _convTime = INA226_CONV_TIME::CONV_TIME_204;
      else if (_stricmp(value, "332") == 0)
        _convTime = INA226_CONV_TIME::CONV_TIME_332;
      else if (_stricmp(value, "588") == 0)
        _convTime = INA226_CONV_TIME::CONV_TIME_588;
      else if (_stricmp(value, "1100") == 0)
        _convTime = INA226_CONV_TIME::CONV_TIME_1100;
      else if (_stricmp(value, "2116") == 0)
        _convTime = INA226_CONV_TIME::CONV_TIME_2116;
      else if (_stricmp(value, "4156") == 0)
        _convTime = INA226_CONV_TIME::CONV_TIME_4156;
      else if (_stricmp(value, "8244") == 0)
        _convTime = INA226_CONV_TIME::CONV_TIME_8244;

    } else if (_stricmp(name, "average") == 0) {
      int avg = _atoi(value);
      switch (avg) {
      case 1:
        _averageMode = INA226_AVERAGES::AVERAGE_1;
        break;
      case 4:
        _averageMode = INA226_AVERAGES::AVERAGE_4;
        break;
      case 16:
        _averageMode = INA226_AVERAGES::AVERAGE_16;
        break;
      case 64:
        _averageMode = INA226_AVERAGES::AVERAGE_64;
        break;
      case 128:
        _averageMode = INA226_AVERAGES::AVERAGE_128;
        break;
      case 256:
        _averageMode = INA226_AVERAGES::AVERAGE_256;
        break;
      case 512:
        _averageMode = INA226_AVERAGES::AVERAGE_512;
        break;
      case 1024:
        _averageMode = INA226_AVERAGES::AVERAGE_1024;
        break;
      default:
        LOGGER_EINFO("Average %d unsupported.", avg);
      }

    } else {
      ret = false;
    } // if

  } // if

  return (ret);
} // set()


/**
 * @brief Activate the INA226Element.
 */
void INA226Element::start() {
  TRACE("start()");

  _sensor = new (std::nothrow) INA226_WE(_address);
  if (_sensor) {
    _sensor->init();

    TRACE(" mode 0x%04x", _mode);
    _sensor->setMeasureMode(_mode);

    TRACE(" shunt %5.2f", _shunt);
    TRACE(" current Range %5.2f", _cRange);
    _sensor->setResistorRange(_shunt, _cRange);

    TRACE(" convTime 0x%04x", _convTime);
    _sensor->setConversionTime(_convTime);

    TRACE(" averageMode 0x%04x", _averageMode);
    _sensor->setAverage(_averageMode);

    SensorElement::start();
  } // if
} // start()


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
bool INA226Element::getProbe(String &values) {
  bool newData = false;
  if (_sensor) {
    char buffer[12 * 3];

    if (_mode == INA226_MEASURE_MODE::TRIGGERED) {
      TRACE("startMeasurement");
      _sensor->startSingleMeasurementNoWait();
    }

    if (_sensor->isBusy()) {
      // wait

    } else if (_sensor->overflow) {
      TRACE("overflow");
      // term();

    } else {
      // read and debug output
      float busVoltage_V = _sensor->getBusVoltage_V();
      float current_mA = _sensor->getCurrent_mA();
      float power_mW = _sensor->getBusPower();

      TRACE("Voltage %5.2f V", busVoltage_V);
      TRACE("Current %5.2f mA", current_mA);
      TRACE("Power   %f mW", power_mW);

      snprintf(buffer, sizeof(buffer), "%.2f,%.2f,%.2f", busVoltage_V, current_mA, power_mW);
      values = buffer;
      newData = true;
    } // if
  } // if (_sensor)
  return (newData);
} // getProbe()


void INA226Element::sendData(String &values) {
  TRACE("data:%s", values.c_str());
  // dispatch values.
  _board->dispatchItem(_voltageAction, values, 0);
  _board->dispatchItem(_currentAction, values, 1);
  _board->dispatchItem(_powerAction, values, 2);
} // sendData()


/**
 * @brief push the current value of all properties to the callback.
 */
void INA226Element::pushState(
    std::function<void(const char *pName, const char *eValue)> callback) {
  Element::pushState(callback);
  callback("voltage", Element::getItemValue(_lastValues, 0).c_str());
  callback("current", Element::getItemValue(_lastValues, 1).c_str());
  callback("power", Element::getItemValue(_lastValues, 2).c_str());
} // pushState()


void INA226Element::term() {
  TRACE("term()");
  SensorElement::term();

  _sensor->powerDown();
  delete _sensor;
  _sensor = nullptr;
} // term()


/* ===== Register the Element ===== */

// As long as the Element is project specific or is a element always used
// the registration is placed here without using a register #define.

// When transferred to the HomeDing library a #define like the
// HOMEDING_INCLUDE_XXX should be used to allow the sketch to select the
// available Elements. See <HomeDing.h> the move these lines to INA226Element.h:

// #ifdef HOMEDING_REGISTER
// Register the INA226Element onto the ElementRegistry.
bool INA226Element::registered =
    ElementRegistry::registerElement("ina226", INA226Element::create);
// #endif

// End
