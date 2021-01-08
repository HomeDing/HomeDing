/**
 * @file BL0937Element.cpp
 * @brief Element to calculate voltage, current and power consumption using the BL0937 chip.
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
 * Changelog:see BL0937Element.h
 */

#include <Arduino.h>
#include <Board.h>
#include <Element.h>

#include <HomeDing.h>

#include "BL0937Element.h"


// ===== meassuring power consumption (CF pin) =====

// static interrupt variables
volatile unsigned long powSigStart;
volatile unsigned long powSigLast;
volatile unsigned int powSigCnt = 0;

// ===== meassuring current/voltage (CF1 pin) =====

// static interrupt variables
volatile unsigned long cf1SigStart;
volatile unsigned long cf1SigLast;
volatile unsigned int cf1SigCnt = 0;

// interrupt routine for power measurement. increment power counter and set exact cycle time.
ICACHE_RAM_ATTR void onPowerSignal()
{
  unsigned long now = micros();
  if (!powSigStart) {
    powSigStart = now;
    powSigCnt = 0;
  } else {
    powSigCnt++;
  }
  powSigLast = now;
} // onPowerSignal


// interrupt routine for current measurement. increment counter and set exact cycle time.
ICACHE_RAM_ATTR void onCF1Signal()
{
  unsigned long now = micros();
  if (!cf1SigStart) {
    cf1SigStart = now;
    cf1SigCnt = 0;
  } else {
    cf1SigCnt++;
  }
  cf1SigLast = now;
} // onCF1Signal


/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new BL0937Element
 * @return BL0937Element* created element
 */
Element *BL0937Element::create()
{
  return (new BL0937Element());
} // create()


/* ===== Element functions ===== */

/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool BL0937Element::set(const char *name, const char *value)
{
  bool ret = true;

  if (_stricmp(name, "mode") == 0) {
    if (_stricmp(value, "current") == 0) {
      _voltageMode = LOW;
    } else if (_stricmp(value, "voltage") == 0) {
      _voltageMode = HIGH;
    }
    if (active) {
      digitalWrite(_pinSel, _voltageMode);
      powSigStart = 0; // start new cycle.
    }

  } else if (_stricmp(name, "selpin") == 0) {
    _pinSel = _atopin(value);

  } else if (_stricmp(name, "cfpin") == 0) {
    _pinCF = _atopin(value);

  } else if (_stricmp(name, "cf1pin") == 0) {
    _pinCF1 = _atopin(value);

  } else if (_stricmp(name, "cycletime") == 0) {
    _cycleTime = _atoi(value);

  } else if (_stricmp(name, "powerfactor") == 0) {
    _powerFactor = strtof(value, nullptr);

  } else if (_stricmp(name, "poweradjust") == 0) {
    float v = strtof(value, nullptr);
    _powerFactor = (v * _powerDuration) / _powerCount;

  } else if (_stricmp(name, "currentfactor") == 0) {
    _currentFactor = strtof(value, nullptr);

  } else if (_stricmp(name, "currentadjust") == 0) {
    float v = strtof(value, nullptr);
    _currentFactor = (v * _cf1Duration) / _cf1Count;

  } else if (_stricmp(name, "voltagefactor") == 0) {
    _voltageFactor = strtof(value, nullptr);

  } else if (_stricmp(name, "voltageadjust") == 0) {
    float v = strtof(value, nullptr);
    _voltageFactor = (v * _cf1Duration) / _cf1Count;

  } else if (_stricmp(name, "onpower") == 0) {
    _powerAction = value;

  } else if (_stricmp(name, "oncurrent") == 0) {
    _currentAction = value;

  } else if (_stricmp(name, "onvoltage") == 0) {
    _voltageAction = value;

  } else {
    ret = Element::set(name, value);
  } // if

  return (ret);
} // set()


/** 
 * @brief Activate the BL0937Element.
 */
void BL0937Element::start()
{
  // TRACE("start()");

  if ((_pinSel < 0) || (_pinCF < 0) || (_pinCF1 < 0)) {
    LOGGER_EERR("configuration incomplete.");

  } else {
    unsigned long now = millis();

    // Verify parameters
    pinMode(_pinCF, INPUT);
    pinMode(_pinCF1, INPUT);
    pinMode(_pinSel, OUTPUT);

    digitalWrite(_pinSel, _voltageMode);

    // start powerCounting
    powSigStart = 0; // start new cycle.
    _cycleStart = now;
    attachInterrupt(_pinCF, onPowerSignal, FALLING);

    attachInterrupt(_pinCF1, onCF1Signal, FALLING);

    // if (parameters ok) {
    Element::start();
  } // if
} // start()


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void BL0937Element::loop()
{
  // do something
  unsigned long now = millis();

  if (now - _cycleStart > _cycleTime) {
    unsigned long newValue = 0;
    unsigned long newCurrentValue = 0;
    unsigned long newVoltageValue = 0;

    // report new power value:
    if (powSigCnt > 2) {
      _powerCount = powSigCnt;
      _powerDuration = (powSigLast - powSigStart);
      newValue = (_powerFactor * _powerCount) / _powerDuration;
    }
    if (_powerValue != newValue) {
      _board->dispatch(_powerAction, newValue);
    }
    _powerValue = newValue;
    powSigStart = 0; // start new cycle.

    // report new cf1 value:
    _cf1Count = cf1SigCnt;
    _cf1Duration = (cf1SigLast - cf1SigStart);
    if (cf1SigCnt > 2) {
      if (_voltageMode) {
        newVoltageValue = (_voltageFactor * _cf1Count) / _cf1Duration;

      } else {
        newCurrentValue = (_currentFactor * _cf1Count) / _cf1Duration;
      } // if
    }
    if (_voltageValue != newVoltageValue) {
      _board->dispatch(_voltageAction, newVoltageValue);
      _voltageValue = newVoltageValue;
    } // if

    if (_currentValue != newCurrentValue) {
      _board->dispatch(_currentAction, newCurrentValue);
      _currentValue = newCurrentValue;
    } // if

    cf1SigStart = 0; // start new cycle.
    _cycleStart = now;
  }
} // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void BL0937Element::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  char buf[38];

  Element::pushState(callback);

  // report actual cycletime and mode
  callback("cycletime", ultoa(_cycleTime, buf, 10));
  callback("mode", _voltageMode ? "voltage" : "current");

  // report actual power and factor in use.
  callback("power", ultoa(_powerValue, buf, 10));
  callback("powerfactor", String(_powerFactor).c_str());

  if (_voltageMode == HIGH) {
    // report actual current and factor in use.
    callback("voltage", ultoa(_voltageValue, buf, 10));
    callback("voltagefactor", String(_voltageFactor).c_str());

  } else {
    // report actual current and factor in use.
    callback("current", ultoa(_currentValue, buf, 10));
    callback("currentfactor", String(_currentFactor).c_str());
  }

  // debug only
  // callback("cnt1", String(cf1SigCnt).c_str());
  // callback("cnt2", String(powSigCnt).c_str());

  // callback("pc", String(_powerCount).c_str());
  // callback("pd", String(_powerDuration).c_str());

} // pushState()


void BL0937Element::term()
{
  TRACE("term()");
  active = false;
} // term()


// End
