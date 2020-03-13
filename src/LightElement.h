/**
 * @file LightElement.h
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
 * * 30.07.2019 created by Matthias Hertel
 * * 15.11.2019 some more modes implemented
 * * 28.12.2019 less blocked time in loop()
 */

#ifndef LIGHTELEMENT_H
#define LIGHTELEMENT_H

#include <Arduino.h>
#include <HomeDing.h>

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
   * @brief initialize a new Element.
   * @param board The board reference.
   */
  virtual void init(Board *board);

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
   * @brief stop all activities and go inactive.
   */
  // virtual void term();

  /**
   * @brief push the current value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void pushState(
      std::function<void(const char *pName, const char *eValue)> callback);

protected:
  /**
   * @brief The actual / new value of the output value.
   */
  String value;

  /**
   * @brief The last / old value of the output value.
   */
  String oldValue;

  /** @brief Brightness level */
  int brightness;

  /** duration of animation / transition in msecs */
  unsigned long duration;

  /** set color pattern */
  void setOutput(String value);

  /**
   * @brief set to true after a color has been prepares to be send to the pixels next loop().
  */
  bool needUpdate;


private:
  /** Number of pixels in the stripe */
  int _count;

  /** data output pins (R,G,B) or pins[0] only. */
  int _pins[4];
};

/* ===== Register the Element ===== */

// HOMEDING_INCLUDE_LIGHT should be used to allow the sketch to select the
// available Elements. See <HomeDing.h> the move these lines to LightElement.h:

#ifdef HOMEDING_REGISTER
// Register the LightElement onto the ElementRegistry.
bool LightElement::registered =
    ElementRegistry::registerElement("light", LightElement::create);
#endif



#endif