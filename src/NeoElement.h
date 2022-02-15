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

#ifndef NEOELEMENT_H
#define NEOELEMENT_H

#include <HomeDing.h>
#include <LightElement.h>

// https://github.com/adafruit/Adafruit_NeoPixel
#include <Adafruit_NeoPixel.h>

/**
 * @brief NeoElement implements an Element to drive LED stripes with the WS2812 LEDs.
 */

class NeoElement : public LightElement
{
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
    color = 0,
    wheel = 1,
    flow = 2,
    pulse = 3,
    more
  };

private:
  /**
   * @brief The actual mode.
  */
  Mode _mode;

  /** Number of pixels in the stripe */
  int _count;

  /** Overall brightness in range 0...100 from LightElement */

  /** data output pin is taken from _pins[0]*/

  /** actual colors are stored in LightElement::value */

  /** duration of animation / transition in msecs */
  unsigned long duration = 4000;

  /** set color pattern */
  void _setColors(String colList);

  Adafruit_NeoPixel *_strip = (Adafruit_NeoPixel *)NULL;

};


/* ===== Register the Element ===== */

// When transferred to the HomeDing library a #define like the
// HOMEDING_INCLUDE_XXX should be used to allow the sketch to select the
// available Elements. See <HomeDing.h> the move these lines to NeoElement.h:

#ifdef HOMEDING_REGISTER
// Register the NeoElement onto the ElementRegistry.
bool NeoElement::registered =
    ElementRegistry::registerElement("neo", NeoElement::create);
#endif

#endif