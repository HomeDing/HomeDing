/**
 * @file DigitalSignalElement.cpp
 * @brief Input Element for the IoT Board Library typically used for digital short spike signals
 * and frequency inputs.
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD 3-Clause style license,
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino.
 *
 * Changelog: see DigitalSignalElement.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <DigitalSignalElement.h>

#define TRACE(...)  // LOGGER_ETRACE(__VA_ARGS__)

/**
 * @brief static factory function to create a new DigitalSignalElement.
 * @return DigitalSignalElement* as Element* created element
 */
Element *DigitalSignalElement::create() {
  return (new DigitalSignalElement());
}  // create()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool DigitalSignalElement::set(const char *name, const char *value) {
  bool ret = true;

  if (_stricmp(name, PROP_PIN) == 0) {
    _pin = _atopin(value);

  } else if (_stricmp(name, "pullup") == 0) {
    _pullup = _atob(value);

  } else if (_stricmp(name, "onHigh") == 0) {
    _highAction = value;

  } else if (_stricmp(name, "onLow") == 0) {
    _lowAction = value;

  } else if (_stricmp(name, ACTION_ONVALUE) == 0) {
    _valueAction = value;

  } else if (_stricmp(name, "duration") == 0) {
    _valueAction = value;

  } else {
    ret = Element::set(name, value);
  }  // if
  return (ret);
}  // set()


/**
 * @brief Activate the DigitalSignalElement.
 */
void DigitalSignalElement::start() {
  // only start with valid pin as input.
  TRACE("start pin=%d", _pin);
  if ((_pin >= 0) && (_usedSignals < 8)) {
    _signalOff = DigitalSignalElement::_usedSignals++;
    pinMode(_pin, _pullup ? INPUT_PULLUP : INPUT);
    // TRACE("offset (%d)", _signalOff);

    if (_signalOff == 0) {
      attachInterrupt(_pin, DigitalSignalElement::onSignal0, CHANGE);
    } else if (_signalOff == 1) {
      attachInterrupt(_pin, DigitalSignalElement::onSignal1, CHANGE);
    } else if (_signalOff == 2) {
      attachInterrupt(_pin, DigitalSignalElement::onSignal2, CHANGE);
    } else if (_signalOff == 3) {
      attachInterrupt(_pin, DigitalSignalElement::onSignal3, CHANGE);
    } else if (_signalOff == 4) {
      attachInterrupt(_pin, DigitalSignalElement::onSignal4, CHANGE);
    } else if (_signalOff == 5) {
      attachInterrupt(_pin, DigitalSignalElement::onSignal5, CHANGE);
    } else if (_signalOff == 6) {
      attachInterrupt(_pin, DigitalSignalElement::onSignal6, CHANGE);
    } else if (_signalOff == 7) {
      attachInterrupt(_pin, DigitalSignalElement::onSignal7, CHANGE);
    }

    Element::start();
  }  // if
}  // start()


/**
 * @brief check the state of the input.
 */
void DigitalSignalElement::loop() {
  unsigned long now = millis();
  unsigned long cnt = DigitalSignalElement::_signalCount[_signalOff];

  // generate _pulse value and actions
  if (cnt != _lastSignalCount) {
    // TRACE("# %ld", cnt);

    // _board->dispatch(_valueAction, cnt);

    if (_pulseStart + _pulseDuration < now) {
      // start impulse output
      _board->dispatch(_valueAction, 1);
      _board->dispatch(_highAction, 1);
      _pulseValue = true;
    }

    _pulseStart = now;
    _lastSignalCount = cnt;

  } else if (_pulseValue) {
    if (_pulseStart + _pulseDuration < now) {
      // start impulse output
      _board->dispatch(_valueAction, 0);
      _board->dispatch(_lowAction, 0);
      _pulseValue = false;
    }  // if
  }    // if
}  // loop()


void DigitalSignalElement::pushState(
  std::function<void(const char *pName, const char *eValue)> callback) {
  Element::pushState(callback);
  callback(PROP_VALUE, _printBoolean(_pulseValue));
}  // pushState()

// ----- static interrupt stuff here -----

uint DigitalSignalElement::_usedSignals = 0;
unsigned long DigitalSignalElement::_signalCount[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

// increment counter on interrupt
IRAM_ATTR void DigitalSignalElement::onSignal0() {
  DigitalSignalElement::_signalCount[0]++;
}
IRAM_ATTR void DigitalSignalElement::onSignal1() {
  DigitalSignalElement::_signalCount[1]++;
}
IRAM_ATTR void DigitalSignalElement::onSignal2() {
  DigitalSignalElement::_signalCount[2]++;
}
IRAM_ATTR void DigitalSignalElement::onSignal3() {
  DigitalSignalElement::_signalCount[3]++;
}
IRAM_ATTR void DigitalSignalElement::onSignal4() {
  DigitalSignalElement::_signalCount[4]++;
}
IRAM_ATTR void DigitalSignalElement::onSignal5() {
  DigitalSignalElement::_signalCount[5]++;
}
IRAM_ATTR void DigitalSignalElement::onSignal6() {
  DigitalSignalElement::_signalCount[6]++;
}
IRAM_ATTR void DigitalSignalElement::onSignal7() {
  DigitalSignalElement::_signalCount[7]++;
}

// End
