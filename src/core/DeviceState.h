/**
 * @file DeviceState.h
 * @author Matthias Hertel (https://www.mathertel.de)
 *
 * @brief This class provides some functions to access the variables in RTC Memory.
 *
 * The Reset counter supports a counter in the range 0..255.
 * The State String supports a string memory up to 200 chars length (199 chars + NUL).
 *
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 *
 * Changelog:
 * * 21.04.2018 creation
 */

#pragma once

class DeviceState {

public:
  /// @brief load preferences from persistent memory
  static void load();

  /// @brief save preferences to persistent memory and clear lastChange
  static void save();

  // return the number of boots since setup of device & storage
  static int getBootCount() {
    return (_bootCount);
  }


  /// @brief Set the Reset Counter object and save all state
  /// @param cnt The new counter value.
  /// @param saveNow
  /// @return int The new counter value.
  static int setResetCounter(int cnt);


  /// @brief return the reset counter value.
  static int getResetCounter() {
    return (_resetCount);
  };


  /// @brief Get the State String object
  /// @return String The actual state string
  static String getStateString() {
    return (_states.concat(VALUE_SEPARATOR));
  };


  /// @brief remove all state information for Elements.
  static void clear() {
    _states.clear();
    _lastChange = millis();
  };


  /// @brief Save state information specific for an Element.
  /// @param e The Element that needs the state support.
  /// @param key Then key of a state variable of the element.
  /// @param value The value of a state variable of the element.
  static void setElementState(Element *e, const char *key, const char *value);


  /// @brief dispatch the saved state to the elements.
  static void loadElementState(Board *board);


  /// @brief return the time of last state changes.
  static unsigned long lastChange() {
    return (_lastChange);
  };


private:
  /// @brief state was loaded from perm. memory.
  static bool _isLoaded;

  /// @brief millis() of last change in state.
  static unsigned long _lastChange;

  // === state variables

  static int _bootCount;
  static int _resetCount;

  // Array of current state properties of Elements.
  static ArrayString _states;

};  // class

// end.
