/**
 * @file ButtonElement.h
 *
 * @brief Core Input Element for the HomeDing Library typically used with
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
 * * 27.11.2018 also can be defined without a physical pin to be controlled by the web ui.
 * * 10.05.2019 reworked to be used as a processing element with no dependencies to pins.
 *              Used state machine from the OneButton library.
 *              Debouncing should be done in the Element that sends the actions to Button.
 */

#ifndef BUTTON_H
#define BUTTON_H

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
   * @brief check the state of the input signal and eventually emit actions.
   */
  virtual void loop();

  /**
   * @brief push the current value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void pushState(
      std::function<void(const char *pName, const char *eValue)> callback);

private:
  bool _inputLevel = 0;

  /** state of the button behavior */
  int _state;

  /** start time of level going HIGH. */
  unsigned long _startTime;

  /**
   * Number of ticks that have to pass by before a long button press is detected
   */
  unsigned long _pressTicks = 1000;

  /**
   * Number of milliseconds that have to pass by before a click is detected.
   */
  unsigned long _clickTicks = 600;


  /**
   * The _clickAction is emitted when the input level has been HIGH(1) then
   * LOW(0) and no other level change has observed since `_clickTicks`.
   */
  String _clickAction;

  /**
   * The _doubleClickAction is emitted
   */
  String _doubleClickAction;

  /**
   * @brief The _pressAction is emitted
   */
  String _pressAction;
};

#ifdef HOMEDING_REGISTER
// Register the ButtonElement onto the ElementRegistry.
bool ButtonElement::registered =
    ElementRegistry::registerElement("button", ButtonElement::create);
#endif

#endif // BUTTON_H