/**
 * @file WordClockElement.h
 * @brief Element Template class.
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
 * * 21.11.2020 created by Matthias Hertel
 */

#include <Arduino.h>
#include <HomeDing.h>


/**
 * @brief The WordClockElement class implements creating patterns for a wordclock
 * using a 8*8 matrix display driven by a MAX7219 chip.
 * @details
@verbatim
This WordClockElement class is not seperated into a *.h and *.cpp file to have a compact implementation.

Thc system configuration is done in env.json file:

* The ntpTime Element ic configured to get the network based time for the local timezone.

Thc device configuration is done in config.json file:

* The time element sends the actual time to the Wordclock element
* The Word clock element creates the pattern on the matrix display and send it to the MAX7219 Element.

The functionality here is inside the `set` and `loop` function. All other functions need not to be implemented.

* The set function retrieves `onValue` action from the settings from the config.json file.
* The set function retrieves retrieves the actual value with the time as a string.
* The loop function checks for a changes time string and send the pattern using the `onValue` action.

@endverbatim
 */


class WordClockElement : public Element {
private:
  String _newTime;
  String _lastTime;
  String _valueAction;

  byte _leds[8];

public:
  /**
   * @brief Factory function to create a WordClockElement.
   * @return Element*
   */
  static Element *create() {
    return (new WordClockElement());
  }  // create()

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;  // initialized later

  /**
   * @brief Set a parameter or property to a new value or start an action.
   * @param name Name of property.
   * @param value Value of property.
   * @return true when property could be changed and the corresponding action
   * could be executed.
   */
  virtual bool set(const char *name, const char *value) override {
    if (_stricmp(name, PROP_VALUE) == 0) {
      LOGGER_EINFO("new Time: %s", value);
      _newTime = value;

    } else if (_stricmp(name, ACTION_ONVALUE) == 0) {
      LOGGER_EINFO("new Action: %s", value);
      _valueAction = value;
    }
  };

#define SWITCH(row, bits) _leds[row] |= bits

  // create and send new WordClock pattern when time has changed.
  virtual void loop() override {
    if (!_newTime.equalsIgnoreCase(_lastTime)) {
      int time = _atotime(_newTime.c_str());
      int hour = time / 60 / 60;
      int min = (time / 60) % 60;
      LOGGER_EINFO("WordTime: %d:%d", hour, min);

      // clear all
      memset(_leds, 0, sizeof(_leds));

      // 5 min intervall only
      min = (min / 5) * 5;

      switch (min) {
        case 0:
          break;
        case 5:
          SWITCH(0, B11110000);  // "Fünf"
          SWITCH(1, B00011110);  // "nach"
          break;
        case 10:
          SWITCH(0, B00001111);  // "Zehn"
          SWITCH(1, B00011110);  // "nach"
          break;
        case 15:
          SWITCH(0, B11111111);  // "Fünfzehn"
          SWITCH(1, B00011110);  // "nach"
          break;
        case 20:
          SWITCH(0, B00001111);  // "Zehn"
          SWITCH(1, B11100000);  // "vor"
          SWITCH(2, B11110000);  // "halb"
          break;
        case 25:
          SWITCH(0, B11110000);  // "Fünf"
          SWITCH(1, B11100000);  // "vor"
          SWITCH(2, B11110000);  // "halb"
          break;
        case 30:
          SWITCH(2, B11110000);  // "halb"
          break;
        case 35:
          SWITCH(0, B11110000);  // "Fünf"
          SWITCH(1, B00011110);  // "nach"
          SWITCH(2, B11110000);  // "halb"
          break;
        case 40:
          SWITCH(0, B00001111);  // "Zehn"
          SWITCH(1, B00011110);  // "nach"
          SWITCH(2, B11110000);  // "halb"
          break;
        case 45:
          SWITCH(0, B11111111);  // "Fünfzehn"
          SWITCH(1, B11100000);  // "vor"
          break;
        case 50:
          SWITCH(0, B00001111);  // "Zehn"
          SWITCH(1, B11100000);  // "vor"
          break;
        case 55:
          SWITCH(0, B11110000);  // "Fünf"
          SWITCH(1, B11100000);  // "vor"
          break;
      }

      if (min >= 20)
        hour++;

      switch (hour % 12) {
        case 0:
          SWITCH(4, B00011111);  // "zwölf"
          break;
        case 1:
          SWITCH(3, B11110000);  // "eins"
          break;
        case 2:
          SWITCH(4, B00011000);
          SWITCH(5, B00000110);  // "zw/ei"
          break;
        case 3:
          SWITCH(5, B00011110);  // "drei"
          break;
        case 4:
          SWITCH(2, B00001111);  // "vier"
          break;
        case 5:
          SWITCH(4, B00000001);
          SWITCH(5, B00000001);
          SWITCH(6, B00000001);
          SWITCH(7, B00000001);  // "fünf"
          break;
        case 6:
          SWITCH(3, B00011111);  // "sechs"
          break;
        case 7:
          SWITCH(4, B11100000);
          SWITCH(5, B11100000);  // "sie/ben"
          break;
        case 8:
          SWITCH(7, B11110000);  // "acht"
          break;
        case 9:
          SWITCH(6, B00001111);  // "neun"
          break;
        case 10:
          SWITCH(6, B01111000);  // "zehn"
          break;
        case 11:
          SWITCH(7, B00000111);  // "elf"
          break;
      }

      char val[20];  // value to be send to matrix display
      sprintf(val, "0x%02x%02x%02x%02x%02x%02x%02x%02x", _leds[0], _leds[1], _leds[2], _leds[3], _leds[4], _leds[5], _leds[6], _leds[7]);
      LOGGER_EINFO("Send: %s", val);
      _board->dispatch(_valueAction, val);

      _lastTime = _newTime;
    }
  };
};

bool WordClockElement::registered = ElementRegistry::registerElement("wordtime", WordClockElement::create);
