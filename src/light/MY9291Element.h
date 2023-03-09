/**
 * @file MY9291Element.h
 * @brief Element for using the Y9291 LED driver chip.
 * 
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD 3-Clause style license,
 * https://www.mathertel.de/License.aspx.
 * 
 * More information on https://www.mathertel.de/Arduino
 * 
 * Changelog:
 * * 30.07.2018 created by Matthias Hertel
 * * 12.05.2021 exclude for ESP32
 * 
    // channel 0 is RED
    // channel 1 is GREEN
    // channel 2 is Blue
    // channel 3 is white
 */

#pragma once

#if defined(ESP8266)

#include <HomeDing.h>
#include <light/LightElement.h>

class MY9291Element : public LightElement
{
public:
  /**
   * @brief Factory function to create a MY9291Element.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;


  /**
   * @brief initialize a new Element.
   * @param board The board reference.
   */
  virtual void init(Board *board) override;

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
   */
  virtual void start() override;

  /**
   * @brief direct function to show a color and brightness.
   */
  virtual void setColor(uint32_t color, int brightness);

private:
    // _pins[0] is used as datapin
    // _pins[1]  is used as clockpin

};

/* ===== Register the Element ===== */

#ifdef HOMEDING_REGISTER
// Register the MY9291Element onto the ElementRegistry.
bool MY9291Element::registered =
    ElementRegistry::registerElement("my9291", MY9291Element::create);
#endif

#elif defined(ESP32)
// No ESP32 based devices known with this chip.
#endif

// End
