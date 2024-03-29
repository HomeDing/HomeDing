/**
 * @file DisplayLCDElement.h
 * @brief Display Element for HD44780 compatible LCD displays.
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
 * @brief DisplayLCDElement implements creating an Display Adapter for a LCD
 * attached to the I2C bus.
 * @details
 * The DisplayLCDAdapter Element includes the real functionality.
 */

class DisplayLCDElement : public DisplayElement {
public:
  /**
   * @brief Factory function to create a DisplayLCDElement.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;

  /**
   * @brief Constructor of a new DisplayLCDElement.
   */
  DisplayLCDElement();

  /**
   * @brief Activate the Element.
   */
  virtual void start() override;
};

#ifdef HOMEDING_REGISTER
// Register the DisplayLCDElement onto the ElementRegistry.
bool DisplayLCDElement::registered =
  ElementRegistry::registerElement("displayLCD", DisplayLCDElement::create);
#endif
