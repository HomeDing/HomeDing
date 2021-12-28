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
#include <MicroJsonComposer.h>

MicroJsonComposer::MicroJsonComposer()
{
  _out.reserve(512);
  _fresh = true;
}

void MicroJsonComposer::openObject()
{
  if (!_fresh) _out.concat(',');
  _out += "{";
  _fresh = true;
}

void MicroJsonComposer::closeObject()
{
  _out += "}";
  _fresh = false;
}

void MicroJsonComposer::openArray()
{
  if (!_fresh) _out.concat(',');
  _out += "[";
  _fresh = true;
}

void MicroJsonComposer::closeArray()
{
  _out += "]";
  _fresh = false;
}


// Create a property with String value
void MicroJsonComposer::addProperty(const char *key, String value)
{
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
void MicroJsonComposer::addProperty(const char *key, const char *value)
{
  addProperty(key, String(value));
}


// Create a property with int value
void MicroJsonComposer::addProperty(const char *key, long value)
{
  addProperty(key, String(value));
};


// Create a property with int value
const char *MicroJsonComposer::stringify()
{
  // _out.replace(",]", "]");
  // _out.replace(",}", "}");
  return (_out.c_str());
};

// end.
