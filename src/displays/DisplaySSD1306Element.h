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
 * * 29.08.2020 based on DisplayElement
 */

#ifndef DisplaySSD1306Element_H
#define DisplaySSD1306Element_H

#include "DisplayElement.h"

/**
 * @brief DisplaySSD1306Element implements creating an Display Adapter for a LCD
 * attached to the I2C bus.
 * @details
 * The DisplayAdapterLCD Element includes the real functionality.
 */

class DisplaySSD1306Element : public DisplayElement
{
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
   * @brief Set a parameter or property to a new value or start an action.
   * @param name Name of property.
   * @param value Value of property.
   * @return true when property could be changed and the corresponding action
   * could be executed.
   */
  virtual bool set(const char *name, const char *value) override;

  /**
   * @brief Activate the Element.
   * @return true when the Element could be activated.
   * @return false when parameters are not usable.
   */
  virtual void start() override;

};

#ifdef HOMEDING_REGISTER
// Register the DisplaySSD1306Element onto the ElementRegistry.
bool DisplaySSD1306Element::registered = ElementRegistry::registerElement(
    "displaySSD1306", DisplaySSD1306Element::create);
#endif

#endif