/**
 * @file DisplaySSD1306Element.h
 * @brief Element Template class.
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

#ifndef DisplaySSD1306Element_H
#define DisplaySSD1306Element_H

#include <Arduino.h>
#include <Element.h>

/**
 * @brief DisplaySSD1306Element implements creating an Display Adapter for a LCD
 * attached to the I2C bus.
 * @details
 * The DisplayAdapterLCD Element includes the real functionality.
 */

class DisplaySSD1306Element : public Element
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
   * @brief Constructor of a new DisplaySSD1306Element.
   */
  DisplaySSD1306Element();

  /**
   * @brief Set a parameter or property to a new value or start an action.
   * @param name Name of property.
   * @param value Value of property.
   * @return true when property could be changed and the corresponding action
   * could be executed.
   */
  virtual bool set(const char *name, const char *value);

  /**
   * @brief Activate the Element.
   * @return true when the Element could be activated.
   * @return false when parameters are not usable.
   */
  virtual void start();

private:
  int _address = 0x3c;
  int _sda = SDA;
  int _scl = SCL;
  int _height = 64;
  int _resetpin = -1;
};

#ifdef HOMEDING_REGISTER
// Register the DisplaySSD1306Element onto the ElementRegistry.
bool DisplaySSD1306Element::registered = ElementRegistry::registerElement(
    "displaySSD1306", DisplaySSD1306Element::create);
#endif

#endif