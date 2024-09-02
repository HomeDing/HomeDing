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
// 02.09.2024 using std::map
// -----

#pragma once

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
   * @return true when registration was successful.
   * @return false when registration failed.
   */
  static bool registerElement(const char *elementTypeName,
                              Element *(*CreateElementFn)(void));

  static Element *createElement(const char *elementTypeName);

  /**
   * @brief List all registered elements in JSON array format:
   * @returns String with all registered element types.
   */
  static String list();
};
