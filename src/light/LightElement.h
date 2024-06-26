/**
 * @file LightElement.h
 * @brief Output Element to drive LEDs on GPIO pins.
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
 * * 10.03.2020 created by Matthias Hertel
 * * 26.01.2021 support WRGB mode (4 LEDs)
 * * 29.05.2023 add invert PWM output
 */

#pragma once

/**
 * @brief LightElement implements an Element to drive a single LED.
 */

class LightElement : public Element
{
public:
  static const int MAXPINS = 4;

  /**
   * @brief Factory function to create a LightElement.
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

  /**
   * @brief push the current value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void pushState(
      std::function<void(const char *pName, const char *eValue)> callback) override;

  /**
   * @brief direct function for Light Elements to show a color and brightness.
   */
  virtual void setColor(uint32_t color, int brightness);

  /**
   * @brief direct function for Light Elements to show a color series and brightness.
   */
  virtual void setColors(uint32_t *color, int brightness);

protected:
  /**
   * @brief The actual / new value of the output value.
   */
  bool enabled = true;

  /**
   * @brief The actual / new value of the output value.
   */
  String value;

  /** @brief Brightness level in percent. 
   * The Light element uses this property as a factor for all PWM output.
  */
  int _brightness = 50; // percent

  /**
   * @brief The actual output color as binary value.
   */
  uint32_t _outColor;

  /**
   * @brief allow direct PWN output for driving LEDs.
   */
  bool pwmMode = false;

  /// @brief invert PWM signals.
  bool invert = false;

  /** data output pins (W,R,G,B) or pins[0] only. */
  int _pins[MAXPINS] = {0, 0, 0, 0}; //  = {D8, D6, D7, 0};

#if (defined(ESP32))
  uint8_t _channels[MAXPINS] = {0, 0, 0, 0}; // led channels for ESP32
#endif

  /**
   * @brief set to true after a color has been prepares to be send to the pixels next loop().
  */
  bool needUpdate = false;


private:
  /** Number of colors / pwm outputs, set by the number of GPIO pins in then pin property. */
  int _count = 0;

};

/* ===== Register the Element ===== */

#ifdef HOMEDING_REGISTER
// Register the LightElement onto the ElementRegistry.
bool LightElement::registered =
    ElementRegistry::registerElement("light", LightElement::create);
#endif
