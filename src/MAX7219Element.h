/**
 * @file MAX7219Element.h
 * @brief Element for driving a MAX7219 chip using SPI.
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
 * * 19.11.2020 created by Matthias Hertel
 * * 24.11.2020 brightness=0 to switch off
*/

#pragma once

class MAX7219Element : public Element
{
public:
  enum class Mode {
    none = 0,
    numeric = 1, // use this mode when max7219 is driving a 7-digit display
    m8x8 = 2 // use this mode when max7219 is driving a 8x8 matrix display
  };

  /**
   * @brief Factory function to create a MAX7219Element.
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
   * @return true when the Element could be activated.
   * @return false when parameters are not usable.
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


private:
  /**
   * @brief Turn all leds off.
   */
  void _clear();

  /**
   * @brief Turn all leds off. brightness = 0 switches off;
   */
  void _setBrightness();

  /**
   * @brief Send data for one line or segment or address.
   */
  void _writeRow(byte row, byte data);

  /**
   * @brief Send a number for 7-segment display.
   */
  void _writeNumber(String number);

  /**
   * @brief Send a bit pattern for 8x8 dot matrix.
   */
  void _writeM8X8(String number);

  /**
   * @brief The actual value to be displayed.
   */
  Mode _mode = Mode::none;

  String _value;
  String _lastValue;

  int _brightness = 7;
  int _csPin = -1;
  int _decodeMode = 0;

  void _write(byte address, byte data);
};


/* ===== Register the Element ===== */


#ifdef HOMEDING_REGISTER

// Register the MAX7219Element onto the ElementRegistry.
bool MAX7219Element::registered =
    ElementRegistry::registerElement("max7219", MAX7219Element::create);
#endif
