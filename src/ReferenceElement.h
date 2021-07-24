/**
 * @file ReferenceElement.h
 * @brief Element that compares the incoming value against a reference value
 * and sends action with value=1 when above the reference value otherwise value=0.
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
 * * 02.12.2021 created by Matthias Hertel
 */

#ifndef REFERENCEELEMENT_H
#define REFERENCEELEMENT_H

#include <cfloat>

class ReferenceElement : public Element
{
public:
  /**
   * @brief Factory function to create a ReferenceElement.
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
  /** The actual incoming value. */
  float _incomingValue;

  /** The actual computed 0/1 value. */
  int _value;

  /** In inverse mode the computed value is inverted. */
  bool _inverse = false;

  /** Set to true when a value was given. */
  bool gotNewValue = false;

  /** The given reference value. */
  float _refValue = 0;

  /** These actions are sent with value=1 when the incoming value is above the reference value otherwise value=0. */
  String _referenceAction;

  /** These actions are sent when the incoming value is above the reference value. */
  String _highAction;

  /** These actions are sent when the incoming value is below or equal the reference value. */
  String _lowAction;
};


#ifdef HOMEDING_REGISTER
// Register the ReferenceElement onto the ElementRegistry.
bool ReferenceElement::registered =
    ElementRegistry::registerElement("reference", ReferenceElement::create);
#endif

#endif