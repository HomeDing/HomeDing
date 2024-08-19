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

  /// @brief reserve at least [num] strings in the array
  /// @param num reserved entries minimum.
  void reserve(uint16_t num = 8);


  /// @brief return true if Array is empty.
  /// @param num reserved entries minimum.
  boolean empty() {
    return (_used == 0);
  }  // empty()


  /// @brief return the number of used Strings in the Array.
  /// @return Number of used Strings.
  uint16_t size() {
    return (_used);
  };

  /// @brief return String by Index
  String at(uint16_t index);

  /// @brief return String by Index
  String operator[](uint16_t index) {
    return (at(index));
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
  int16_t push(const char *s) {
    setAt(_used, s);
    return (_used);
  };  // push()


  /// @brief Add new String to the end of the array
  /// @param s new item
  /// @return size of the array.
  int16_t push(const String &s) {
    return (push(s.c_str()));
  };


  /// @brief build the array by splitting a string into parts.
  /// @param s String with input
  /// @param delim Delimiter character to be used.
  void split(const char *s, char delim = ',');

  /// @brief build the array by splitting a string into parts.
  /// @param s String with input
  /// @param delim Delimiter character to be used.
  void split(const String &s, char delim = ',') {
    split(s.c_str(), delim);
  }


  /// @brief Find an entry starting with the given string.
  /// @param sFind string to be found
  /// @return index of first found entry or -1 for not found.
  int16_t findStartWith(const String &sFind);


  /// @brief remove first string from array and return it
  String pop();

  /// @brief remove n-th entry from array and return it
  String remove(uint16_t n);

  /// @brief deallocate all memory and empty array.
  void clear();

  /// @brief Create a string with all array items separatend by delim.
  String concat(char delim);

  /// @brief Dump of the ArrayString object to output.
  void dump();

private:
  /// @brief allocated capacity for array intems (char* pointers).
  uint16_t _capacity = 0;
  /// @brief number of used items [0...(_used-1)].
  uint16_t _used = 0;

  char **array = nullptr;  // array with pointers to C-type strings

  /// @brief make sure that enough array pointers are given.
  void _createCapacity(uint16_t num);
};

// End
