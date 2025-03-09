/**
 * @file MicroJsonComposer.cpp
 * @brief String based buffer to create JSON objects.
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
 * Changelog:see MicroJsonComposer.h
 */

#include <Arduino.h>
#include <HomeDing.h>
#include <MicroJsonComposer.h>

#define TRACE(...) Serial.sprintf(__VA_ARGS__)

MicroJsonComposer::MicroJsonComposer() {
  _out.reserve(512);
  _fresh = true;
}

void MicroJsonComposer::openObject() {
  if (!_fresh) _out.concat(',');
  _out += "{";
  _fresh = true;
}

// Create a property with String value
void MicroJsonComposer::addObject(const char *id) {
  CTRACE("addObject(%s)\n", id);

  _out.reserve(_out.length() + strlen(id) + 32);
  if (!_fresh) _out.concat(',');
  _out.concat('\"');
  _out.concat(id);
  _out.concat("\":{");
  _fresh = true;
};

void MicroJsonComposer::closeObject() {
  _out += "}";
  _fresh = false;
}

void MicroJsonComposer::openArray() {
  if (!_fresh) _out.concat(',');
  _out += "[";
  _fresh = true;
}

void MicroJsonComposer::addConstant(const char *value) {
  _out.reserve((_out.length() + strlen(value) + 64) & 0xFFFFC0);
  if (!_fresh) _out.concat(',');
  _out.concat('\"');
  _out.concat(value);
  _out.concat('\"');
  _fresh = false;
}

void MicroJsonComposer::closeArray() {
  _out += "]";
  _fresh = false;
}


// Create a property with String value
void MicroJsonComposer::addProperty(const char *key, String value) {
  CTRACE("addProperty(%s)\n", key);

  _out.reserve((_out.length() + strlen(key) + value.length() + 64) & 0xFFFFC0);
  if (!_fresh) _out.concat(',');
  value.replace("\"", "\\\"");
  _out.concat('\"');
  _out.concat(key);
  _out.concat("\":\"");
  _out.concat(value);
  _out.concat('\"');
  _fresh = false;
};

// Create a property with char* value
void MicroJsonComposer::addProperty(const char *key, const char *value) {
  addProperty(key, String(value));
};


// Create a property with int value
void MicroJsonComposer::addProperty(const char *key, long value) {
  addProperty(key, String(value));
};


// Return the composed string
const char *MicroJsonComposer::stringify() {
  return (_out.c_str());
};

// end.
