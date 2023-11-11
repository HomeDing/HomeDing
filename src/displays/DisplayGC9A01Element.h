/**
 * @file DisplayGC9A01Element.h
 * @brief Display Element for GC9A01 compatible TFT displays.
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
 * * 11.11.2023 created by Matthias Hertel
 */

#pragma once

#include "DisplayElement.h"

/**
 * @brief DisplayGC9A01Element implements creating an Display Adapter for a LCD
 * attached to the I2C bus.
 * @details
 * The DisplayAdapter Element includes the real functionality.
 */

class DisplayGC9A01Element : public DisplayElement {
public:
  /**
   * @brief Factory function to create a DisplayGC9A01Element.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;


  /// @brief initialize a new Element.
  /// @param board The board reference.
  virtual void init(Board *board) override;

  //// @brief Activate the Element.
  virtual void start() override;
};


#ifdef HOMEDING_REGISTER
// Register the DisplayGC9A01Element onto the ElementRegistry.
bool DisplayGC9A01Element::registered =
  ElementRegistry::registerElement("DisplayGC9A01", DisplayGC9A01Element::create);
#endif
