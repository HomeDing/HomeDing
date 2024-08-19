/**
 * @file DisplayST7701Element.h
 * @brief Display Element for ST7701 compatible TFT displays using the 16 bit panel interface.
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
 * * 27.01.2024 created by Matthias Hertel
 */

#pragma once

#include <displays/DisplayElement.h>

/// @brief DisplayST7701Element implements creating an Display Adapter for a ST7701 based LCD.
class DisplayST7701Element : public DisplayElement {
public:
  /**
   * @brief Factory function to create a DisplayST7701Element.
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
// Register the DisplayST7701Element onto the ElementRegistry.
bool DisplayST7701Element::registered =
  ElementRegistry::registerElement("displayst7701", DisplayST7701Element::create);
#endif
