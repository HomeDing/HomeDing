/**
 * @file NeoElement.h
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

class Adafruit_NeoPixel; // forward

/**
 * @brief NeoElement implements an Element to drive LED stripes with the WS2812 LEDs.
 */

class NeoElement : public LightElement {
public:
  /**
   * @brief Factory function to create a NeoElement.
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
   * @brief Give some processing time to the timer to check for next action.
   */
  virtual void loop() override;


  enum class Mode {
    _min = 0,      // minimum value
    _default = 0,  // default value

    fix = 0,       // take inbound value for output
    flow = 1,      // rainbow colors flowing

    _max = 1  // maximum value
  };

  /** set a single color for all neopixels */
  virtual void setColor(uint32_t color, int brightness) override;

private:
  /**
   * @brief The actual mode.
   */
  Mode _mode = Mode::_default;

  /** Number of pixels in the stripe */
  int _count = 1;

  /** Config of pixels order */
  int _config;

  /** Overall brightness in range 0...100 from LightElement */

  /** data output pin is taken from _pins[0]*/

  /** actual colors are stored in LightElement::value */

  /** duration of animation / transition in msecs */
  unsigned long duration = 4000;

  Adafruit_NeoPixel *_strip = (Adafruit_NeoPixel *)NULL;
};


/* ===== Register the Element ===== */

#ifdef HOMEDING_REGISTER
// Register the NeoElement onto the ElementRegistry.
bool NeoElement::registered =
  ElementRegistry::registerElement("neo", NeoElement::create);
#endif
