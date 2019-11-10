
// MicroJsonComposer.h
//
// ===== JSON Composer =====

#include <Arduino.h>
#include <MicroJsonComposer.h>

MicroJsonComposer::MicroJsonComposer()
{
  _out.reserve(512);
  _fresh = true;
}

void MicroJsonComposer::openObject()
{
  if (!_fresh) _out.concat(',');;
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

// Create a property with int value
void MicroJsonComposer::addProperty(const char *key, int value)
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
