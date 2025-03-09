// -----
// ElementRegistry.cpp -
//
// Copyright (c) by Matthias Hertel, https://www.mathertel.de.
//
// This work is licensed under a BSD style license.
// See https://www.mathertel.de/License.aspx.
// More information on https://www.mathertel.de/Arduino
// -----
// Changelog: see ElementRegistry.h
// -----

#include <Arduino.h>
#include <HomeDing.h>
#include <MicroJsonComposer.h>

#include <core/Logger.h>

// allocate static variables
int ElementRegistry::_count;
const char *ElementRegistry::_names[REG_MAX_TYPES];
CreateElementFn ElementRegistry::_func[REG_MAX_TYPES];


/**
 * @brief Register a Factoryunction to create a Element of the specific type.
 */
bool ElementRegistry::registerElement(const char *elementTypeName,
                                      Element *(*CreateElementFn)(void)) {
  // This functio is called during static variable initialization. Serial
  // doesn't work so early: LOGGER_RAW("register(%s)", elementTypeName);
  if (_count < REG_MAX_TYPES - 1) {
    _names[_count] = elementTypeName;
    _func[_count] = CreateElementFn;
    _count++;
    return (true);
  }
  return (false);
}  // registerElement()


Element *ElementRegistry::createElement(const char *elementTypeName) {
  // LOGGER_RAW("createElement(%s)", elementTypeName);
  int n;
  Element *e = NULL;

  // search for the the typeName
  n = 0;
  while ((n < _count) && (!String(_names[n]).equalsIgnoreCase(elementTypeName))) {
    n++;
  }  // while

  if (n < _count) {
    e = _func[n]();
  }  // if
  return (e);
}  // createElement()


/**
 * @brief List all registered elements in JSON array format and return as String.
 */
String ElementRegistry::list() {
  MicroJsonComposer jc;
  jc.openArray();
  for (int n = 0; n < _count; n++) {
    jc.addConstant(_names[n]);
  }  // for
  jc.closeArray();
  return (jc.stringify());
}  // list()

// End