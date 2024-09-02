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
#include <map>

#include <core/Logger.h>


static std::map<const char *, CreateElementFn, _pchar_less> hdRegistry;

/**
 * @brief Register a Factoryunction to create a Element of the specific type.
 */
bool ElementRegistry::registerElement(
  const char *elementTypeName,
  Element *(*CreateElementFn)(void)) {

  // This function is called during static variable initialization. Serial
  // doesn't work so early: LOGGER_RAW("register(%s)", elementTypeName);
  // hdRegistry.insert(std::make_pair(elementTypeName, CreateElementFn));
  hdRegistry.insert({elementTypeName, CreateElementFn});

  return (true);
}  // registerElement()


Element *ElementRegistry::createElement(const char *elementTypeName) {
  // LOGGER_RAW("createElement(%s)", elementTypeName);
  auto p = hdRegistry.find(elementTypeName);
  return ((p != hdRegistry.end()) ? p->second() : nullptr);
}  // createElement()


/**
 * @brief List all registered elements in JSON array format:
 * @param buffer for the names.
 */
String ElementRegistry::list() {
  String buffer = "[";

  for (auto p = hdRegistry.begin(); p != hdRegistry.end(); ++p) {
    buffer.concat('\"');
    buffer.concat(p->first);
    buffer.concat('\"');
    if (p != hdRegistry.end()) buffer.concat(',');
  }
  buffer.concat(']');
  return (buffer);
}  // list()

// End
