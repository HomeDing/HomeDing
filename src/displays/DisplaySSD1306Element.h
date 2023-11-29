/**
 * @file DisplaySSD1306Element.h
 * @brief Display Element for SSD1306 based OLED displays.
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
 * * 27.11.2023 Using GFX for Arduino Library
 */

#pragma once

#include "DisplayElement.h"

/**
 * @brief DisplaySSD1306Element implements creating an Display Adapter for a OLED
 * attached to the I2C bus.
 * @details
 * The DisplaySD1306Adapter includes the real functionality.
 */

class DisplaySSD1306Element : public DisplayElement {
public:
  /* ===== Static factory function ===== */

  /**
   * @brief Factory function to create a DisplaySSD1306Element.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;

  /* ===== Element functions ===== */

  /**
   * @brief Activate the Element.
   */
  virtual void start() override;
};

#ifdef HOMEDING_REGISTER
// Register the DisplaySSD1306Element onto the ElementRegistry.
bool DisplaySSD1306Element::registered =
  ElementRegistry::registerElement("displaySSD1306", DisplaySSD1306Element::create);
#endif
