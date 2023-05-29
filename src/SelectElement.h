/**
 * @file SelectElement.h
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:
 * * 30.01.2022 created by Matthias Hertel
 */

#pragma once

/**
 * @brief Element for selecting one of many options with key and value.
 */
class SelectElement : public Element {
public:
  /**
   * @brief Factory function to create a SelectElement.
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
   * @brief push the current value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void pushState(
    std::function<void(const char *pName, const char *eValue)> callback) override;

private:
  /**
   * @brief select the option and dispatch all actions.
   * @param n index of new option
   */
  void _selectOption(int n);

  /**
   * @brief The current selected index.
   */
  int _selected = -1;

  /// turn around at the end with next and prev
  boolean _cycle = false;

  /// @brief List of keys.
  ArrayString _keys;

  /// @brief List of values.
  ArrayString _values;

  /// @brief Action to be sent with key as a parameter
  String _keyAction;

  /// @brief Action to be sent with value as a parameter
  String _valueAction;
};

/* ===== Register the Element ===== */

#ifdef HOMEDING_REGISTER
// Register the SelectElement in the ElementRegistry.
bool SelectElement::registered =
  ElementRegistry::registerElement("select", SelectElement::create);
#endif
