/**
 * @file DisplaySH1106Element.h
 * @brief Display Element for SH1106 based OLED displays.
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
 * @brief DisplaySH1106Element implements creating an Display Adapter for a LCD
 * attached to the I2C bus.
 * @details
 * The DisplaySH1106Adapter Element includes the real functionality.
 */

class DisplaySH1106Element : public DisplayElement {
public:
  /* ===== Static factory function ===== */

  /**
   * @brief Factory function to create a DisplaySH1106Element.
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
// Register the DisplaySH1106Element onto the ElementRegistry.
bool DisplaySH1106Element::registered = ElementRegistry::registerElement(
  "displaySH1106", DisplaySH1106Element::create);
#endif
