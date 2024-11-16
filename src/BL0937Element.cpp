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
#include <HomeDing.h>

#include <BL0937Element.h>

// enable TRACE for sending detailed output from this Element
#define TRACE(...)  // LOGGER_ETRACE(__VA_ARGS__)

// ===== meassuring power consumption (CF pin) =====

// static interrupt variables
unsigned long powSigStart;
unsigned long powSigLast;
unsigned int powSigCnt = 0;

// ===== meassuring power consumption in Wh (CF pin) =====

unsigned long energyCount = 0;  // counting PowerSignal pulses

time_t energyDate = 0;                 // start of reporting day.
unsigned long energyCountLastDay = 0;  // from yesterday

// the power pulse count (onPowerSignal) is directly proportional to energy.
// x [W] = (_powerFactor * _powerCount) / _powerDuration [nsec];
// x [Wh] = (_powerFactor * energyCount / 3600) / 1000000;

// ===== meassuring current/voltage (CF1 pin) =====

// static interrupt variables
unsigned long cf1SigStart;
unsigned long cf1SigLast;
unsigned int cf1SigCnt = 0;

// interrupt routine for power measurement. increment power counter and set exact cycle time.
IRAM_ATTR void onPowerSignal() {
  unsigned long now = micros();
  if (!powSigStart) {
    powSigStart = now;
    powSigCnt = 0;
  } else {
    powSigCnt++;
  }
  energyCount++;
  powSigLast = now;
}  // onPowerSignal


// interrupt routine for current measurement. increment counter and set exact cycle time.
IRAM_ATTR void onCF1Signal() {
  unsigned long now = micros();
  if (!cf1SigStart) {
    cf1SigStart = now;
    cf1SigCnt = 0;
  } else {
    cf1SigCnt++;
  }
  cf1SigLast = now;
}  // onCF1Signal


/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new BL0937Element
 * @return BL0937Element* created element
 */
Element *BL0937Element::create() {
  return (new BL0937Element());
}  // create()


/* ===== Element functions ===== */

/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool BL0937Element::set(const char *name, const char *value) {
  bool ret = true;

  if (name == HomeDing::Actions::Mode) {
    _voltageMode = (_stricmp(value, "current") != 0);

    if (active) {
      digitalWrite(_pinSel, _voltageMode);
      powSigStart = 0;  // start new cycle.
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

  } else if (_stricmp(name, "onEnergy") == 0) {
    _energyAction = value;

  } else {
    ret = Element::set(name, value);
  }  // if

  return (ret);
}  // set()


/**
 * @brief Activate the BL0937Element.
 */
void BL0937Element::start() {
  // TRACE("start()");

  if ((_pinSel < 0) || (_pinCF < 0) || (_pinCF1 < 0)) {
    LOGGER_EERR("configuration incomplete");

  } else {
    unsigned long now = millis();

    // Verify parameters
    pinMode(_pinCF, INPUT);
    pinMode(_pinCF1, INPUT);
    pinMode(_pinSel, OUTPUT);

    digitalWrite(_pinSel, _voltageMode);

    // start powerCounting
    powSigStart = 0;  // start new cycle.
    _cycleStart = now;

    // start powerCounting per day
    energyCount = 0;
    energyDate = time(nullptr) - Board::getTimeOfDay();

    attachInterrupt(_pinCF, onPowerSignal, FALLING);
    attachInterrupt(_pinCF1, onCF1Signal, FALLING);

    Element::start();

#if 0
// Example on accessing the current data of an element
// by url: http://<devicename>/bl
    _board->server->on("/bl", HTTP_GET, [this]() {
      String ret;
      ret += "time: " + String(time(nullptr)) + "\n";
      ret += "tod: " + String(Board::getTimeOfDay()) + "\n";
      ret += "energyDate: " + String(energyDate) + "\n";
      ret += "d: " + String(time(nullptr) - energyDate);
      _board->server->send(200, "text/plain", ret.c_str());
    });
#endif

  }  // if
}  // start()


/** Give some processing time check for actions. */
void BL0937Element::loop() {

  // Last day is over.
  if ((time(nullptr) - energyDate) > (24 * 60 * 60)) {
    // report day power consumption by resporting the pulse counts of the day
    // as pulse are proportional to the used energy.

    energyCountLastDay = energyCount;

    float energyFactor = _powerFactor / 3600 / 1000000;
    float wh = energyCount * energyFactor;
    TRACE("last day energy: %d,%energyFactor", energyDate, wh);
    HomeDing::Actions::push(_energyAction, String(wh));

    // start powerCounting per day
    energyCount = 0;
    energyDate = time(nullptr) - Board::getTimeOfDay();
  }  // if


  // Cycle is over.
  if (_board->nowMillis - _cycleStart > _cycleTime) {
    unsigned long newPowerValue = 0;
    unsigned long newVAValue = 0;  // voltage or Ampere

    // report new power value:
    if (powSigCnt > 2) {
      _powerCount = powSigCnt;
      _powerDuration = (powSigLast - powSigStart);
      newPowerValue = (_powerFactor * _powerCount) / _powerDuration;
    }
    if (_powerValue != newPowerValue) {
      HomeDing::Actions::push(_powerAction, newPowerValue);
    }
    _powerValue = newPowerValue;
    powSigStart = 0;  // start new cycle.

    // report new cf1 value:
    _cf1Count = cf1SigCnt;
    _cf1Duration = (cf1SigLast - cf1SigStart);

    if (cf1SigCnt > 2) {
      newVAValue = ((_voltageMode ? _voltageFactor : _currentFactor) * _cf1Count) / _cf1Duration;
    }

    if (_voltageValue != newVAValue) {
      HomeDing::Actions::push(_voltageAction, newVAValue);
      _voltageValue = newVAValue;
    }  // if

    if (_currentValue != newVAValue) {
      HomeDing::Actions::push(_currentAction, newVAValue);
      _currentValue = newVAValue;
    }  // if

    cf1SigStart = 0;  // start new cycle.
    _cycleStart = _board->nowMillis;
  }  // if

}  // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void BL0937Element::pushState(
  std::function<void(const char *pName, const char *eValue)> callback) {
  Element::pushState(callback);

  // report actual cycletime and mode
  callback("cycletime", _printInteger(_cycleTime));
  callback("mode", _voltageMode ? "voltage" : "current");

  // report actual power and factor in use.
  callback("power", _printInteger(_powerValue));
  callback("powerfactor", String(_powerFactor).c_str());

  float energyFactor = _powerFactor / 3600 / 1000000;
  callback("energy", String(energyCount * energyFactor).c_str());
  callback("lastDay", String(energyCountLastDay * energyFactor).c_str());

  if (_voltageMode) {
    // report actual current and factor in use.
    callback("voltage", _printInteger(_voltageValue));
    callback("voltagefactor", String(_voltageFactor).c_str());

  } else {
    // report actual current and factor in use.
    callback("current", _printInteger(_currentValue));
    callback("currentfactor", String(_currentFactor).c_str());
  }
}  // pushState()

// End
