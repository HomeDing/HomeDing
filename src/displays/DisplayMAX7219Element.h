/**
 * @file DisplayMAX7219Element.h
 * @brief Display Element for MAX7219 compatible LED matrix displays.
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
 * * 23.03.2022 created by Matthias Hertel
 */

#pragma once

#include "DisplayElement.h"

/**
 * @brief DisplayMAX7219Element implements creating an Display Adapter for
 * LED Matrix based on MAX7219 elements.
 * @details
 * The DisplayLCDAdapter Element includes the real functionality.
 */

class DisplayMAX7219Element : public DisplayElement {
public:
  /**
   * @brief Factory function to create a DisplayMAX7219Element.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;

  /**
   * @brief Activate the Element.
   */
  virtual void start() override;
};

#ifdef HOMEDING_REGISTER
// Register the DisplayMAX7219Element onto the ElementRegistry.
bool DisplayMAX7219Element::registered =
  ElementRegistry::registerElement("DisplayMAX7219", DisplayMAX7219Element::create);
#endif
