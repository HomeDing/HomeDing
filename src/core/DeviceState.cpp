/**
 * @file RTCVariables.cpp
 * @author Matthias Hertel (https://www.mathertel.de)
 *
 * @brief This class implements some functions to access the variables in RTC Memory for ESP8266.
 *
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 *
 * Changelog: see RTCVariables.h
 */

#include <Arduino.h>
#include <HomeDing.h>
#include "hdfs.h"

#define PREF_FILENAME "/$pref.txt"

// use BOARDTRACE for compiling with detailed TRACE output.
#define STATETRACE(...) LOGGER_TRACE(__VA_ARGS__)

/// @brief state was loaded from perm. memory.
bool DeviceState::_isLoaded = false; 

/// @brief millis() of last change in state.
unsigned long DeviceState::_lastChange = 0;


// default values used when no PREF file was found.
int DeviceState::_bootCount = 0;
int DeviceState::_resetCount = 0;
String DeviceState::_state;


/// @brief Initialize local static variables.
void DeviceState::load() {
  STATETRACE("init()");
  if (!_isLoaded) {
    File f = HomeDingFS::rootFS->open(PREF_FILENAME, "r");

    if (!f) {
      STATETRACE("no PREF File");

    } else {
      String s = f.readStringUntil('\n');
      _bootCount = s.toInt();
      _bootCount++;
      _lastChange = millis();

      s = f.readStringUntil('\n');
      // STATETRACE("rc=<%s>", s.c_str());
      _resetCount = s.toInt();

      _state = f.readStringUntil('\n');

      while (1) {
        s = f.readStringUntil('\n');
        if (s.isEmpty()) break;

        STATETRACE("+<%s>", s.c_str());
      };

      f.close();
    }
    _isLoaded = true;
    STATETRACE("boot=%d, reset=%d", _bootCount, _resetCount);
  }
}


/// @brief save local static variables.
void DeviceState::save() {
  STATETRACE("save(%d)", _lastChange > 0);
  if (_lastChange) {

    File f = HomeDingFS::rootFS->open(PREF_FILENAME, "w");

    if (f) {
      f.println(_bootCount);
      f.println(_resetCount);
      f.println(_state);
      f.close();
    }

    _lastChange = 0;
  }
}  // save()


/**
 * @brief Set the Reset Counter object
 */
int DeviceState::setResetCounter(int cnt) {
  STATETRACE("setResetCounter(%d)", cnt);
  // The Reset counter supports a counter in the range 0..255
  // that is stored in the lowest byte of the 32-bit value.
  // The higher 3 bytes are used to check for a valid entry.

  if (_isLoaded) {
    _resetCount = constrain(cnt, 0, 255);
    _lastChange = millis();
    save();
  }
  return (cnt);
}  // setResetCounter


/// @brief Save state information specific for an Element.
/// @param element The Element that needs the state support.
/// @param key Then key of a state variable of the element.
/// @param value The value of a state variable of the element.
void DeviceState::setElementState(Element *e, const char *key, const char *value) {
  STATETRACE("setElementState(%s, %s, %s)", e->id, key, value);

  String entry = String(e->id) + "?" + key + "=";

  int n = _state.indexOf(entry);
  entry.concat(value);

  if (n < 0) {
    // add to the end
    if (_state.length() > 0)
      _state.concat(VALUE_SEPARATOR);
    _state.concat(entry);

  } else {
    String tmp;

    if (n > 0) {
      tmp = _state.substring(0, n); // all actions before incl. VALUE_SEPARATOR
    }
    tmp.concat(entry);

    int e = _state.indexOf(VALUE_SEPARATOR, n);
    if (e >= 0) {
      tmp.concat(_state.substring(e));
    }
    _state = tmp;
  } // if
  _lastChange = millis();
}


/// @brief Load all state information from RTC memory and dispatch them as actions.
void DeviceState::loadElementState(Board *board){
  STATETRACE("loadElementState()");

  String loadState = _state;

  while (loadState.length() > 0) {
    String a = Element::popItemValue(loadState);
    // _board->dispatchAction(a);
  } // while

};

// End.
