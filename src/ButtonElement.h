/**
 * @file ButtonElement.h
 *
 * @brief Core Input Element for the HomeDing Library typicaly used with
 * momentary buttons or switches.
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
 */

#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>
#include <Element.h>

#define BUTTON_TYPE_LEVEL  0x00
#define BUTTON_TYPE_TOGGLE 0x01

// class OneButton;

/**
 * @brief The ButtonElement is an special Element that creates actions based on
 * a digital IO signal.
 */
class ButtonElement : public Element
{
public:
  /**
   * @brief Factory function to create a ButtonElement.
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
   * @brief check the state of the input signal and evenuallt emit actions.
   */
  virtual void loop();

  /**
   * @brief push the current value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void pushState(
      std::function<void(const char *pName, const char *eValue)> callback);

private:
  int _type = BUTTON_TYPE_LEVEL;
  int _pin = -1;
  bool _invers = false;
  bool _pullup = false;

  //  OneButton *_button = NULL;
  int _lastInLevel;
  int _lastOutLevel;

  /**
   * @brief The _onAction is emitted when the logical input level is going from
   * LOW to HIGH. With type == TOGGLE this action will be emitted every second
   * time when the input level is going from HIGH to LOW (Button is released).
   */
  String _onAction;

  /**
   * @brief The _offAction is emitted when the logical input level is going from
   * HIGH to LOW. With type == TOGGLE this action will be emitted every second
   * time when the input level is going from HIGH to LOW (Button is released).
   */
  String _offAction;
};

#ifdef HOMEDING_REGISTER
// Register the ButtonElement onto the ElementRegistry.
bool ButtonElement::registered =
    ElementRegistry::registerElement("button", ButtonElement::create);
#endif

#endif // BUTTON_H