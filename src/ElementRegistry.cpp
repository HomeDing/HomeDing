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

#include "ElementRegistry.h"
#include "Element.h"

#define LOGGER_MODULE "Registry"
#include "Logger.h"


// allocate static variables
int ElementRegistry::_count;
const char *ElementRegistry::_names[20];
CreateElementFn ElementRegistry::_func[20];


/**
 * @brief Register a Factoryunction to create a Element of the specific type.
 */
bool ElementRegistry::registerElement(const char *elementTypeName,
                                      Element *(*CreateElementFn)(void))
{
  // This functio is called during static variable initialization. Serial
  // doesn't work so early: LOGGER_TRACE("register(%s)", elementTypeName);
  if (_count < 19) {
    _names[_count] = elementTypeName;
    _func[_count] = CreateElementFn;
    _count++;
    return (true);
  }
  return (false);
} // registerElement()


Element *ElementRegistry::createElement(const char *elementTypeName)
{
  static bool done_once = false;
  LOGGER_TRACE("createElement(%s)", elementTypeName);
  int n;
  Element *e = NULL;

  if (!done_once) {
    // report registered libraries
    String s;
    for (n = 0; n < _count; n++) {
      s.concat(" ");
      s.concat(_names[n]);
    } // for
    LOGGER_INFO("Registered Libraries:%s\n", s.c_str());
    done_once = true;
  } // if

  // search for the the typeName
  n = 0;
  while ((n < _count) &&
         (!String(_names[n]).equalsIgnoreCase(elementTypeName))) {
    n++;
  } // while

  if (n < _count) {
    e = _func[n]();
  } // if
  return (e);
} // createElement()

// End
