/**
 * @file AndElement.h
 * @brief Logical Element that combines boolean input values using the AND and optional NOT operator and sends actions.
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
 * * 17.07.2019 created by Matthias Hertel
 */

#ifndef ANDELEMENT_H
#define ANDELEMENT_H

#define AndElementInputs 2

class AndElement : public Element
{
public:
  /**
   * @brief Factory function to create a AndElement.
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
  virtual bool set(const char *name, const char *value);

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
   * @brief The actual input values.
   */
  bool _value[AndElementInputs];
  bool _outValue;
  bool _invert = false;

  /**
   * @brief The _valueAction holds the actions that is submitted when ...
   */
  String _valueAction;
};

/* ===== Register the Element ===== */

// As long as the Element is project specific or is a element always used
// the registration is placed here without using a register #define.

// When transferred to the HomeDing library a #define like the
// HOMEDING_INCLUDE_My should be used to allow the sketch to select the
// available Elements. See <HomeDing.h> the move these lines to AndElement.h:

#ifdef HOMEDING_REGISTER
// Register the AndElement onto the ElementRegistry.
bool AndElement::registered =
    ElementRegistry::registerElement("and", AndElement::create);
#endif

#endif