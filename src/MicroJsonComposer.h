/**
 * @file MicroJsonComposer.cpp
 * @brief String based buffer to create JSON objects.
 *
 * This class creates a JSON formatted string by appending JSON arrays, objects and properties.
 * Not all combinations of array / object nesting is supported as of now.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license.
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:
 * * 11.10.2019 created by Matthias Hertel
 */

#pragma once

class MicroJsonComposer {
public:
  MicroJsonComposer(int size = 300);

  // Add a new unnamed object in root or array
  void openObject();
  void addObject(const char *id);
  void closeObject();

  // Add a new unnamed object in root or array
  void openArray();

  // add a String value to array
  void addConstant(const char *value);

  void closeArray();

  
  // Create a property with String value
  void addProperty(const char *key, String value);

  // Create a property with int value
  void addProperty(const char *key, long value);

  // Create a property with char* value
  void addProperty(const char *key, const char *value);

  const char *stringify();

protected:
  // result buffer
  String _out;

  // fresh is true when an object/array was just opened
  bool _fresh;
};

// end.
