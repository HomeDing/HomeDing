/**
 * @file DisplayESP32PanelElement.h
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

#if defined(ESP32) && defined(CONFIG_IDF_TARGET_ESP32S3)

#include <displays/DisplayElement.h>

/// @brief DisplayESP32PanelElement implements creating an Display Adapter for a ST7796 based LCD.
class DisplayESP32PanelElement : public DisplayElement {
public:
  /**
   * @brief Factory function to create a DisplayESP32PanelElement.
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
// Register the DisplayESP32PanelElement onto the ElementRegistry.
bool DisplayESP32PanelElement::registered =
  ElementRegistry::registerElement("displayesp32panel", DisplayESP32PanelElement::create);
#endif

#endif