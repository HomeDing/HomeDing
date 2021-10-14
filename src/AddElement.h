/**
 * @file AddElement.h
 * @brief Logical Base Element that collects input values and settings.
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
 * * 13.10.2021 created by Matthias Hertel
 */

#ifndef ADDELEMENT_H
#define ADDELEMENT_H

#include <HomeDing.h>

#define MAX_INPUTS 8

class AddElement : public Element
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
   * @brief Activate the Element.
   */
  virtual void start();

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
   * @brief push the current outgoing value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void pushState(
      std::function<void(const char *pName, const char *eValue)> callback) override;

protected:
  /// number of given input values.
  int  _inputs = 0;

  /// flag set to true for the need of re-calculation of the output.
  bool _needRecalc;

  /// current input values.
  int _inValue[MAX_INPUTS];

  /// current output value.
  int _value;

  /// The _valueAction holds the actions that is submitted when outvalue has changed.
  String _valueAction;
};


#ifdef HOMEDING_REGISTER
// Register the AddElement in the ElementRegistry.
bool AddElement::registered =
    ElementRegistry::registerElement("add", AddElement::create);
#endif


#endif