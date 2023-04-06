/**
 * @file APA102Element.h
 * @brief Output Element to control neopixel / WS2812 based LED stripes on a GPIO pin.
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
 * * 30.07.2019 created by Matthias Hertel
 * * 15.11.2019 some more modes implemented
 * * 28.12.2019 less blocked time in loop()
 */

#pragma once

#include <HomeDing.h>
#include <light/LightElement.h>
#include <light/StripeElement.h>

/**
 * @brief APA102Element implements an Element to drive LED stripes with the APA102 LEDs.
 */

class APA102Element : public StripeElement {
public:
  /**
   * @brief Factory function to create a APA102Element.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;

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
   * @brief Give some processing time to the timer to check for next action.
   */
  virtual void loop() override;

  void show();

private:
};


/* ===== Register the Element ===== */

#ifdef HOMEDING_REGISTER
// Register the APA102Element onto the ElementRegistry.
bool APA102Element::registered =
  ElementRegistry::registerElement("apa102", APA102Element::create);
#endif
