// -----
// ElementRegistry.h - Registry of all included Element classes to allow further
// creation of Elements by name.
//
// Copyright (c) by Matthias Hertel, https://www.mathertel.de.
//
// This work is licensed under a BSD style license.
// See https://www.mathertel.de/License.aspx.
// More information on https://www.mathertel.de/Arduino
// -----
// 31.05.2018 created by Matthias Hertel
// -----

#ifndef ELEMENTREGISTRY_H
#define ELEMENTREGISTRY_H

#include "Element.h"
#include <Arduino.h>

#define REG_MAX_TYPES 32

typedef Element *(*CreateElementFn)(void);

/**
 * @brief Registration of Element Types to avoid hard references.
 *
 * Element types need to register themselves into the ElementRegistry to make
 * them available for use in the board configuration.
 */
class ElementRegistry
{
public:
  /**
   * @brief Register a Factoryunction to create a Element of the specific type.
   * @param elementTypeName Name of the registered Element type.
   * @param CreateElementFn  Factory Function to create a new Element.
   * @return true when registration was sucessful.
   * @return false when registration failed.
   */
  static bool registerElement(const char *elementTypeName,
                              Element *(*CreateElementFn)(void),
                              bool isSingleton = false);

  static Element *createElement(const char *elementTypeName);

private:
  static int _count;
  static const char *_names[REG_MAX_TYPES];
  static CreateElementFn _func[REG_MAX_TYPES];
  static bool _singleton[REG_MAX_TYPES];

};


#endif