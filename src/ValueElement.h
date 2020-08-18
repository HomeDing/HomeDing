/**
 * @file ValueElement.h
 * @brief The ValueElement combines receiving modifying actions for an value and sending actions on changing the value.
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
 * * 31.07.2018 min and max added.
 * * 06.09.2019 derivation of switch element
 */

#ifndef ValueELEMENT_H
#define ValueELEMENT_H

#include <HomeDing.h>

/**
 * @brief Timer Element that produces actions based on timing conditions.
 * @details
@verbatim

The ValueElement can ...

@endverbatim
 */

class ValueElement : public Element
{
public:
  /**
   * @brief Factory function to create a ValueElement.
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
   * @brief Activate the Element.
   * @return true when activation was good.
   * @return false when activation failed.
   */
  virtual void start();

  /**
   * @brief push the current value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void pushState(
      std::function<void(const char *pName, const char *eValue)> callback);

  int getValue();
  const char *getLabel();

protected:
  bool _isValid = false;

  int _minRange = -2147483648;
  int _maxRange = 2147483647;
  int _step = 1;

  /**
   * @brief The _valueAction holds the actions that is submitted when the value has changed.
   */
  String _valueAction;

  virtual bool _setValue(int val, bool forceAction = false);
  virtual bool _setValue(const char *val, bool forceAction = false);

  virtual int _getValueInt();

private:
  /**
   * @brief The actual value.
   */
  int _value = 0;
  String _valueString;

  String _label;

  bool _isStringType = false;

};

#ifdef HOMEDING_REGISTER
// Register the ValueElement in the ElementRegistry.
bool ValueElement::registered =
    ElementRegistry::registerElement("value", ValueElement::create);
#endif

#endif