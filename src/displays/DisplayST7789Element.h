/**
 * @file DisplayST7789Element.h
 * @brief Display Element for ST7789 compatible TFT displays.
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
 * @brief DisplayST7789Element implements creating an Display Adapter for a LCD
 * attached to the I2C bus.
 * @details
 * The DisplayAdapter Element includes the real functionality.
 */

class DisplayST7789Element : public DisplayElement {
public:
  /**
   * @brief Factory function to create a DisplayST7789Element.
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
// Register the DisplaySSD1306Element onto the ElementRegistry.
bool DisplayST7789Element::registered =
  ElementRegistry::registerElement("DisplayST7789", DisplayST7789Element::create);
#endif
