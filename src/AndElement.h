/**
 * @file AndElement.h
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
 * * 17.07.2019 created by Matthias Hertel
 */

#ifndef ANDELEMENT_H
#define ANDELEMENT_H

/**
 * @brief AndElement implements...
 * @details
@verbatim

The AndElement can ...

@endverbatim
 */

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

#endif