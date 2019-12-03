/**
 * @file PMSElement.h
 * @brief Element Template class.
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
 * * 30.07.2018 created by Matthias Hertel
 * * 01.12.2019 enable/disable rx instead of begin/end.
 * * 01.12.2019 use onValue not onChange (as usual)
 */

#ifndef PMSELEMENT_H
#define PMSELEMENT_H

#include <HomeDing.h>
#include <SoftwareSerial.h>

/**
 * @brief PMSElement implements...
 * @details
@verbatim

The PMSElement can ...

@endverbatim
 */

class PMSElement : public Element
{
public:
  /**
   * @brief Factory function to create a PMSElement.
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
  // virtual void init(Board *board);

  /**
   * @brief Set a parameter or property to a new value or start an action.
   * @param name Name of property.
   * @param value Value of property.
   * @return true when property could be changed and the corresponding action
   * could be executed.
   */
  virtual bool set(const char *name, const char *value);

  /**
   * @brief Activate the Element.
   * @return true when the Element could be activated.
   * @return false when parameters are not usable.
   */
  virtual void start();

  /**
   * @brief Give some processing time to the timer to check for next action.
   */
  virtual void loop();

  /**
   * @brief push the current value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void pushState(
      std::function<void(const char *pName, const char *eValue)> callback);

private:
  /**
   * @brief The actual value.
   */
  int _value;

  int _pinrx = D4;
  int _pintx = D3;

  unsigned long _readTime = 60;
  unsigned long _nextRead;

  // buffer for data from sensor 
  uint8_t _data[32];

  int _datapos; // current position in input buffer, -1 means not receiving

  SoftwareSerial *_pmsSerial;

  /**
   * @brief The _changeAction holds the actions that is submitted when new data was received from the sensor.
   */
  String _valueAction;
};

/* ===== Register the Element ===== */

#ifdef HOMEDING_REGISTER
bool PMSElement::registered =
    ElementRegistry::registerElement("pms", PMSElement::create);
#endif

#endif