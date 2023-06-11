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
 * * 07.06.2023 migrated from Adafruit_GFX to Arduino_GFX
 */

#pragma once

#include "DisplayElement.h"

/// @brief The DisplayST7735Element implements configutation and creation of a Display Adapter
//  for a LCD attached to the SPI bus.
class DisplayST7735Element : public DisplayElement {
public:

  /// @brief Factory function to create a DisplayST7735Element.
  /// @return created Element* 
  static Element *create();

  /// @brief static variable to support registtration in the static init phase.
  static bool registered;

  /// @brief Activate the Element.
  virtual void start() override;
};

#ifdef HOMEDING_REGISTER
// Register the DisplayST7735Element onto the ElementRegistry.
bool DisplayST7735Element::registered =
  ElementRegistry::registerElement("DisplayST7735", DisplayST7735Element::create);
#endif
