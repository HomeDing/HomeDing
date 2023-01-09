/**
 * @file TM1637Element.h
 * @brief The TM1637Element implements communication with TM1637 LED driver chips to implement 7 segment LED display.
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
 * * 12.01.2021 created by Matthias Hertel
 * * implementing a single chip only. chained chips not yet supported.
 */

#ifndef TM1637ELEMENT_H
#define TM1637ELEMENT_H

#include <HomeDing.h>

/**
 * @brief The TM1637Element implements communication with TM1637 LED driver chips.
 */

class TM1637Element : public Element {
public:
  /**
   * @brief Factory function to create a TM1637Element.
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
  // === configurations

  uint32_t _ioDelay;     //< microseconds to wait before next signal change
  int _type;             //< tmxx type of display
  unsigned int _digits;  //< number of digits the display supports

  int _dataPin = -1;   //< pin for the data signal (DIO)
  int _clockPin = -1;  //< pin for the clock signal (clk)
  int _csPin = -1;     // //< pin for the select signal (CS, STB)

  bool _needUpdate;     //< true when next loop must output the data
  int _brightness = 2;  //< brightness for next display data
  String _value;        //< current dispay value

  // segments for numbers 0..9
  static const int _numSegments[10];

  // Display a number or a time from a string.
  void _display(const char *s);

  // display raw data
  void _displayRaw(int *data);

  /**
   * @brief Send a sequence of data and clock signals.
   * @param seq Sequence as String.
   * @param data 1 byte data to be send.
   */
  int _io(const char *seq, int data = 0);
};

/* ===== Register the Element ===== */

#ifdef HOMEDING_REGISTER

// Register the TM1637Element onto the ElementRegistry.
bool TM1637Element::registered =
  ElementRegistry::registerElement("tm1637", TM1637Element::create);
#endif


#endif