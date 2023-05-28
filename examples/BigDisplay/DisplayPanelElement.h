/**
 * @file DisplayPanelElement.h
 * @brief Display Element for Panel compatible TFT displays.
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

#include <displays/DisplayElement.h>

/**
 * @brief DisplayPanelElement implements creating an Display Adapter for a LCD
 * attached to the RGB Panel bus on EPS32S3.
 * @details
 * The DisplayAdapter Element includes the real functionality.
 */

class DisplayPanelElement : public DisplayElement {
public:
  /**
   * @brief Factory function to create a DisplayPanelElement.
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
// Register the DisplayPanelElement onto the ElementRegistry.
bool DisplayPanelElement::registered =
  ElementRegistry::registerElement("DisplayPanel", DisplayPanelElement::create);
#endif
