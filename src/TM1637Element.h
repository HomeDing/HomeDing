/**
 * @file TM1637Element.h
 * @brief The P8913Element implements communication with P9813 LED driver chips to implement RGB lights.
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

#include "tm1637.h"

/**
 * @brief The P8913Element implements communication with P9813 LED driver chips to implement RGB lights.
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
  bool _needUpdate;
  int _brightness;
  int _dataPin;
  int _clockPin;
  String _value;

  TM1637 *_disp;
};

/* ===== Register the Element ===== */

#ifdef HOMEDING_REGISTER

// Register the TM1637Element onto the ElementRegistry.
bool TM1637Element::registered =
    ElementRegistry::registerElement("tm1637", TM1637Element::create);
#endif


#endif