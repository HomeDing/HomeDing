/**
 * @file DisplayST7796Element.h
 * @brief Display Element for ST7796 compatible TFT displays.
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
 * * 27.05.2023 created by Matthias Hertel
 */

#pragma once

#include <displays/DisplayElement.h>

/// @brief DisplayST7796Element implements creating an Display Adapter for a ST7796 based LCD.
class DisplayST7796Element : public DisplayElement {
public:
  /**
   * @brief Factory function to create a DisplayST7796Element.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;

  virtual void init(Board *board) override;

  /**
   * @brief Activate the Element.
   */
  virtual void start() override;
};

#ifdef HOMEDING_REGISTER
// Register the DisplayST7796Element onto the ElementRegistry.
bool DisplayST7796Element::registered =
  ElementRegistry::registerElement("displayst7796", DisplayST7796Element::create);
#endif
