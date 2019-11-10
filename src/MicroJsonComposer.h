
// MicroJsonComposer.h
//
// ===== JSON Composer =====
//
// This class creates a JSON formatted string by appending JSON arrays, objects and properties.


#ifndef MICROJSONC_H
#define MICROJSONC_H

class MicroJsonComposer
{
public:
  MicroJsonComposer();

  void openObject();
  void closeObject();

  void openArray();
  void closeArray();

  // Create a property with String value
  void addProperty(const char *key, String value);

  // Create a property with int value
  void addProperty(const char *key, int value);

  const char *stringify();

protected:
  String _out;
  bool _fresh;
};


#endif

// end.
