/**
 * @file INA219Element.cpp
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
 * Changelog:see INA219Element.h
 */

// https://arduino-projekte.webnode.at/meine-libraries/stromsensor-ina219/
// https://learn.adafruit.com/adafruit-ina219-current-sensor-breakout?view=all


#include <Arduino.h>
#include <Element.h>
#include <Board.h>

#include <HomeDing.h>

#include "INA219Element.h"


/* ===== Define local constants and often used strings ===== */

// like:
// #define ANYCONSTANT 1000


/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new INA219Element
 * @return INA219Element* created element
 */
Element *INA219Element::create()
{
  return (new INA219Element());
} // create()


/* ===== Element functions ===== */

INA219Element::INA219Element()
{
  // adjust startupMode when Network (default) is not applicable.
  // startupMode = Element_StartupMode::System;
}


void INA219Element::init(Board *board)
{
  TRACE("init()");
  SensorElement::init(board);
} // init()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool INA219Element::set(const char *name, const char *value)
{
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

    } else if (_stricmp(name, "gain") == 0) {
      if (_stricmp(value, "40") == 0)
        _gain = INA219_PGAIN::PG_40;
      else if (_stricmp(value, "80") == 0)
        _gain = INA219_PGAIN::PG_80;
      else if (_stricmp(value, "160") == 0)
        _gain = INA219_PGAIN::PG_160;
      else if (_stricmp(value, "320") == 0)
        _gain = INA219_PGAIN::PG_320;

    } else if (_stricmp(name, "mode") == 0) {
      if (_stricmp(value, "once") == 0)
        _mode = INA219_MEASURE_MODE::TRIGGERED;
      else if (_stricmp(value, "continuous") == 0)
        _mode = INA219_MEASURE_MODE::CONTINUOUS;

    } else if (_stricmp(name, "samples") == 0) {
      if (_stricmp(value, "9bit") == 0)
        _samples = INA219_ADC_MODE::BIT_MODE_9;
      else if (_stricmp(value, "10bit") == 0)
        _samples = INA219_ADC_MODE::BIT_MODE_10;
      else if (_stricmp(value, "11bit") == 0)
        _samples = INA219_ADC_MODE::BIT_MODE_11;
      else if (_stricmp(value, "12bit") == 0)
        _samples = INA219_ADC_MODE::BIT_MODE_12;

      else if (_stricmp(value, "2") == 0)
        _samples = INA219_ADC_MODE::SAMPLE_MODE_2;
      else if (_stricmp(value, "4") == 0)
        _samples = INA219_ADC_MODE::SAMPLE_MODE_4;
      else if (_stricmp(value, "8") == 0)
        _samples = INA219_ADC_MODE::SAMPLE_MODE_8;
      else if (_stricmp(value, "16") == 0)
        _samples = INA219_ADC_MODE::SAMPLE_MODE_16;
      else if (_stricmp(value, "32") == 0)
        _samples = INA219_ADC_MODE::SAMPLE_MODE_32;
      else if (_stricmp(value, "64") == 0)
        _samples = INA219_ADC_MODE::SAMPLE_MODE_64;
      else if (_stricmp(value, "128") == 0)
        _samples = INA219_ADC_MODE::SAMPLE_MODE_128;

    } else if (_stricmp(name, "range") == 0) {
      if (_stricmp(value, "16") == 0)
        _range = INA219_BUS_RANGE::BRNG_16;
      else if (_stricmp(value, "32") == 0)
        _range = INA219_BUS_RANGE::BRNG_32;

    } else {
      ret = false;
    } // if

  } // if

  return (ret);
} // set()


/**
 * @brief Activate the INA219Element.
 */
void INA219Element::start()
{
  TRACE("start()");

  _sensor = new (std::nothrow) INA219_WE(0x40);
  if (_sensor) {
    _sensor->init();

    LOGGER_EINFO(" mode 0x%04x", _mode);
    _sensor->setMeasureMode(_mode);

    LOGGER_EINFO(" gain 0x%04x", _gain);
    _sensor->setPGain(_gain);

    LOGGER_EINFO(" range 0x%04x", _range);
    _sensor->setBusRange(_range);

    LOGGER_EINFO(" samples 0x%04x", _samples);
    _sensor->setADCMode(_samples);

    SensorElement::start();
  } // if

} // start()


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
bool INA219Element::getProbe(String &values)
{
  bool newData = false;
  char buffer[12 * 3];

  if (_mode == INA219_MEASURE_MODE::TRIGGERED) {
    _sensor->startSingleMeasurement();
  }

  if (_sensor->getOverflow()) {
    LOGGER_EERR("overflow");
    term();
  } else {

    // read and debug output
    float busVoltage_V = _sensor->getBusVoltage_V();
    float current_mA = _sensor->getCurrent_mA();
    float power_mW = _sensor->getBusPower();

    // LOGGER_EINFO("Voltage %5.2f V", busVoltage_V);
    // LOGGER_EINFO("Current %5.2f mA", current_mA);
    // LOGGER_EINFO("Power   %5.2f mW", power_mW);

    snprintf(buffer, sizeof(buffer), "%.2f,%.2f,%.2f", busVoltage_V, current_mA, power_mW);
    values = buffer;
    bool newData = true;
  } // if
  return (true);
} // loop()


void INA219Element::sendData(String &values)
{
  TRACE("data:%s", values.c_str());
  // dispatch values.
  _board->dispatchItem(_voltageAction, values, 0);
  _board->dispatchItem(_currentAction, values, 1);
  _board->dispatchItem(_powerAction, values, 2);
} // sendData()


/**
 * @brief push the current value of all properties to the callback.
 */
void INA219Element::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
  callback(PROP_VALUE, String(_value).c_str());
} // pushState()


void INA219Element::term()
{
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
// available Elements. See <HomeDing.h> the move these lines to INA219Element.h:

// #ifdef HOMEDING_REGISTER
// Register the INA219Element onto the ElementRegistry.
bool INA219Element::registered =
    ElementRegistry::registerElement("ina219", INA219Element::create);
// #endif

// End
