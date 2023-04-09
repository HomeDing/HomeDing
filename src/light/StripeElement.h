/**
 * @file StripeElement.h
 * @brief Base Element for LED stripes.
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
 * * 04.04.2023 created by Matthias Hertel
 */

#pragma once

#include <HomeDing.h>
#include <light/LightElement.h>

#define STRIPE_DATA_PIN _pins[0]
#define STRIPE_CLOCK_PIN _pins[1]


/**
 * @brief StripeElement implements an Element to drive LED stripes with the WS2812 LEDs.
 */

class StripeElement : public LightElement {
public:
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

  /**
   * @brief push the current value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void pushState(
    std::function<void(const char *pName, const char *eValue)> callback) override;

  /**
   * @brief update stripe with brightnes and colors from pixel array.
   */
  virtual void show();

  enum class Mode {
    _min = 0,      // minimum value
    _default = 0,  // default value

    fix = 0,     // take inbound value for output
    single = 1,  // use a single color.
    flow = 2,    // rainbow colors flowing

    _max = 2  // maximum value
  };

  /** set a single color for all pixels in the stripe */
  virtual void setColor(uint32_t color, int brightness) override;

protected:
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

  uint32_t *pixels;  // List of RGB or WRGB values for LEDs

  /** duration of animation / transition in msecs */
  unsigned long duration = 4000;
};
