/**
 * @file ArrayString.h
 *
 * @brief Array of Strings, stored by using minimal heap consumption in c-style.
 * With operators for array and queues.
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
 * 10.05.2023 created.
 */

#pragma once

#include <Arduino.h>

class ArrayString {

public:
  ArrayString(){};

  ~ArrayString() {
    clear();
  };

  // reserve at least [num] strings in the array
  void reserve(uint16_t num);

  boolean empty() {
    return (_used == 0);
  }

  uint16_t size() {
    return (_used);
  }

  // return String by Index
  String at(uint16_t index);

  // return String by index
  String operator[](uint16_t index) {
    return (at(index));
  };

  // return first String
  String first() {
    return (at(0));
  };

  // add a String at the given position
  void setAt(uint16_t index, const char *s);

  // add a String at the given position
  void setAt(uint16_t index, const String &s) {
    setAt(index, s.c_str());
  };


  /// @brief Add new String to the end of the array
  /// @param s new item
  /// @return size of the array.
  uint16_t push(const char *s) {
    setAt(_used, s);
    return (_used);
  };  // push()


  /// @brief Add new String to the end of the array
  /// @param s new item
  /// @return size of the array.
  uint16_t push(const String &s) {
    return (push(s.c_str()));
  };


  void split(const char *s, char delim = ',');

  void split(const String &s, char delim = ',') {
    split(s.c_str(), delim);
  }

  // void split(String s, char delim = ',') {
  //   split(s.c_str(), delim);
  // }

  // remove first string from array and return it
  String pop();

  /// @brief deallocate all memory and empty array.
  void clear();

  /// @brief Create a string with all array items separatend by delim.
  String concat(char delim);

  // send a dump of the object to Serial.
  void dump();

private:
  uint16_t _capacity = 0;
  uint16_t _used = 0;

  char **array = nullptr;  // array with pointers to C-type strings

  // make sure that enough array pointers are given.
  void _createCapacity(uint16_t num);
};

// End
