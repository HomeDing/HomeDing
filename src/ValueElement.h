/**
 * @file ValueElement.h
 * @brief Element TODO: description
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
 */

#ifndef ValueELEMENT_H
#define ValueELEMENT_H

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

private:
  /**
   * @brief The actual value.
   */
  int _value = 0;

  int _min = -2147483648;
  int _max = 2147483647;
  int _step = 1;

  /**
   * @brief The _xAction holds the actions that is submitted when ...
   */
  String _valueAction;

  void _setValue(int newValue);
};

#ifdef HOMEDING_REGISTER
// Register the ValueElement in the ElementRegistry.
bool ValueElement::registered =
    ElementRegistry::registerElement("value", ValueElement::create);
#endif

#endif