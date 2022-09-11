/**
 * @file TheColorAnimationElement.h
 * @brief The ColorAnimation Element creates a series or pattern of color values.
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
 * * 28.12.2019 created by Matthias Hertel
 * * 28.12.2019 take over modes from neo element.
 * * 13.02.2022 pass through brightness.
 */

#pragma once

#include <vector>

#include <HomeDing.h>
#include <light/LightElement.h>

/**
 * @brief ColorElement implements...
 * @details
@verbatim

The ColorAnimation Element creates a series or pattern of color values that can vary over time.
This is typically used for controlling multiple light emitting devices.

@endverbatim
 */

class ColorElement : public Element {
public:
  /**
   * @brief Factory function to create a ColorElement.
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
   * @brief Give some processing time to the timer to check for next action.
   */
  virtual void loop() override;

  /**
   * @brief push the current value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void pushState(
    std::function<void(const char *pName, const char *eValue)> callback) override;

private:
  enum class Mode {
    _min = 0,      // minimum value
    _default = 0,  // default value

    fix = 0,       // take inbound value for output
    fade = 1,      // fade to inbound value from current value
    wheel = 2,     // single color output cycling through whole hue cycle
    pulse = 3,

    _max = 3  // maximum value
  };


  /**
   * @brief The actual output value.
   */
  uint32_t _value = 0;


  boolean _needUpdate = false;


  /** @brief  The actual brightness output.
   * The Light element uses this property as a factor for all PWM output.
   */
  int _brightness = 50;  // percent

  /**
   * @brief The values for a transition.
   */
  uint32_t _fromValue = 0;
  uint32_t _toValue = 0;

  /**
   * @brief The duration of one animation cycle or transition in milliSeconds.
   */
  unsigned long _duration = 10 * 1000;

  /**
   * @brief The time when the animation was started in milliSeconds.
   */
  unsigned long _startTime;

  /**
   * @brief The duration of one animation cycle or transition in milliSeconds.
   */
  Mode _mode = Mode::_default;

  /**
   * @brief The _valueAction holds the actions that is submitted when the color changes.
   */
  String _valueAction;

  /**
   * @brief linked elements
   */
  std::vector<LightElement *> _lightElements;  // direct linked elements

  /**
   * @brief The _brightnessAction holds the actions that is submitted when the brightness changes.
   */
  String _brightnessAction;
};

#ifdef HOMEDING_REGISTER
// Register the ColorElement in the ElementRegistry.
bool ColorElement::registered =
  ElementRegistry::registerElement("color", ColorElement::create);
#endif
