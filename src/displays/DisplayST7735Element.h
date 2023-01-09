/**
 * @file DisplayST7735Element.h
 * @brief Display Element for ST7735 compatible LCD displays.
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
 * * 18.09.2018 created by Matthias Hertel
 */

#pragma once

#include "DisplayElement.h"

/**
 * @brief DisplayST7735Element implements creating an Display Adapter for a LCD
 * attached to the I2C bus.
 * @details
 * The DisplayST7735Adapter includes the real functionality.
 */

class DisplayST7735Element : public DisplayElement {
public:
  /**
   * @brief Factory function to create a DisplayST7735Element.
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
// Register the DisplayST7735Element onto the ElementRegistry.
bool DisplayST7735Element::registered =
  ElementRegistry::registerElement("DisplayST7735", DisplayST7735Element::create);
#endif
