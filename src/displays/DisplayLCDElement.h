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

#ifndef DisplayLCDElement_H
#define DisplayLCDElement_H

#include "DisplayElement.h"

/**
 * @brief DisplayLCDElement implements creating an Display Adapter for a LCD
 * attached to the I2C bus.
 * @details
 * The DisplayAdapterLCD Element includes the real functionality.
 */

class DisplayLCDElement : public DisplayElement
{
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
bool DisplayLCDElement::registered =
    ElementRegistry::registerElement("displayLCD", DisplayLCDElement::create);
#endif


#endif