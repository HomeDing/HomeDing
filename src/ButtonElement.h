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
 * * 18.08.2019 Settings in tick for timing
 * * 18.08.2019 Remote actions
 */

#ifndef BUTTON_H
#define BUTTON_H

#include <HomeDing.h>

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
  virtual bool set(const char *name, const char *value) override;

  /**
   * @brief check the state of the input signal and eventually emit actions.
   */
  virtual void loop() override;

  /**
   * @brief push the current value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void pushState(
      std::function<void(const char *pName, const char *eValue)> callback) override;

private:
  /** Actions */
  typedef enum {
    ACTION_CLICK = 1,
    ACTION_DOUBLECLICK = 2,
    ACTION_PRESS = 9
  } ACTIONS;

  bool _inputLevel = 0;

  /** state of the button behavior */
  int _state;

  /** start time of level going HIGH. */
  unsigned long _startTime;

  /**
   * Number of ticks that have to pass by before a long button press is detected
   */
  unsigned long _pressTicks = 800;

  /**
   * Number of milliseconds that have to pass by before a click is detected.
   */
  unsigned long _clickTicks = 250;


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

  /**
   * @brief The _actionAction is emitted
   */
  String _actionAction;

  /**
   * @brief ...
   */
  void _send(ACTIONS action);
};

#ifdef HOMEDING_REGISTER
// Register the ButtonElement onto the ElementRegistry.
bool ButtonElement::registered =
    ElementRegistry::registerElement("button", ButtonElement::create);
#endif

#endif // BUTTON_H