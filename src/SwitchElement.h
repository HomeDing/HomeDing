/**
 * @file SwitchElement.h
 * @brief Element switching a digital signal HIGH LOW based on actions.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license,
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino.
 *
 * Changelog:
 * * 29.04.2018 created by Matthias Hertel
 * * 16.09.2018 pullup config added.
 * * 27.11.2018 also can be defined without a physical pin to be controlled by
 * the web ui.
 */

#ifndef SWITCH_H
#define SWITCH_H

/**
 * @brief The SwitchElement is an special Element that creates actions based on
 * a digital IO signal.
 */
class SwitchElement : public Element
{
public:
  /**
   * @brief Factory function to create a SwitchElement.
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
   * @brief push the current value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void pushState(
      std::function<void(const char *pName, const char *eValue)> callback);

private:
  // current output value of the switch.
  bool _value = false;

  /**
   * @brief The _onAction is emitted when the logical output level is going from
   * LOW to HIGH.
   */
  String _highAction;

  /**
   * @brief The _offAction is emitted when the logical output level is going from
   * HIGH to LOW.
   */
  String _lowAction;

  /**
   * @brief The _valueAction is emitted when the logical output level changed.
   */
  String _valueAction;

  void _newValue(bool val);
};

#ifdef HOMEDING_REGISTER
// Register the SwitchElement onto the ElementRegistry.
bool SwitchElement::registered =
    ElementRegistry::registerElement("switch", SwitchElement::create);
#endif

#endif // SWITCH_H