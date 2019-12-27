/**
 * @file ButtonElement.cpp
 * @brief Input Element for the IoT Board Library typically used with momentary
 * buttons or switches.
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license,
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino.
 *
 * Changelog: see ButtonElement.h
 */

#include <Arduino.h>
#include <Board.h>
#include <Element.h>

#include "ButtonElement.h"


#define STATE_INIT 0 // waiting for input
#define STATE_HIGH1 1 // got a first high
#define STATE_LOW1 2 // went low again
#define STATE_HIGH2 3 // went up a second time shortly after the first
#define STATE_PRESSHIGH 6 // is high since a long time.

/** The TRACE Macro is used for trace output for development/debugging purpose. */
#define TRACE(...) LOGGER_ETRACE(__VA_ARGS__)
// #define TRACE(...)


/**
 * @brief static factory function to create a new ButtonElement.
 * @return ButtonElement* as Element* created element
 */
Element *ButtonElement::create()
{
  return (new ButtonElement());
} // create()

// http://lcddevice/$board/neo/d3?color=0
// http://lcddevice/$board/button/on?action=click

/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool ButtonElement::set(const char *name, const char *value)
{
  bool ret = true;
  TRACE("set %s=%s", name, value);

  if (_stricmp(name, PROP_VALUE) == 0) {
    _inputLevel = _atob(value);

  } else if (_stricmp(name, "action") == 0) {
    // received an action that will be processedn
    if (_stricmp(value, "click") == 0) {
      _send(ACTION_CLICK);
    } else if (_stricmp(value, "doubleclick") == 0) {
      _send(ACTION_DOUBLECLICK);
    } else if (_stricmp(value, "press") == 0) {
      _send(ACTION_PRESS);
    }

  } else if (_stricmp(name, "clickticks") == 0) {
    _clickTicks = atoi(value);

  } else if (_stricmp(name, "pressticks") == 0) {
    _pressTicks = atoi(value);

  } else if (_stricmp(name, "onclick") == 0) {
    _clickAction = value;

  } else if (_stricmp(name, "ondoubleclick") == 0) {
    _doubleClickAction = value;

  } else if (_stricmp(name, "onpress") == 0) {
    _pressAction = value;

  } else if (_stricmp(name, "onaction") == 0) {
    _actionAction = value;

  } else {
    ret = Element::set(name, value);
  } // if
  return (ret);
} // set()


/**
 * @brief Activate the ButtonElement.
 */
void ButtonElement::start()
{
  Element::start();
} // start()


/**
 * @brief check the input level.
 */
void ButtonElement::loop()
{
  unsigned long now = millis(); // current (relative) time in msecs.

  // if (_state != STATE_INIT)
  //   LOGGER_ETRACE("loop-%d)", _state);

  // state machine
  if (_state == STATE_INIT) { // waiting for menu pin being pressed.

    if (_inputLevel) {
      _state = STATE_HIGH1; // step to state 1
      _startTime = now; // remember starting time
    } // if

  } else if (_state == STATE_HIGH1) { // waiting for menu pin being released.
    if (!_inputLevel) {
      _state = STATE_LOW1;

    } else if ((_inputLevel) &&
               ((unsigned long)(now - _startTime) > _pressTicks)) {
      _send(ACTION_PRESS);
      _state = STATE_PRESSHIGH;

    } else {
      // wait. Stay in this state.
    } // if

  } else if (_state == STATE_LOW1) {
    // waiting for menu pin being pressed the second time or timeout.

    // if (_doubleClickAction.length() == 0 || ...
    if ((unsigned long)(now - _startTime) > _clickTicks) {
      // this was only a single short click
      _send(ACTION_CLICK);
      _state = STATE_INIT; // restart.

    } else if (_inputLevel) {
      _state = STATE_HIGH2;
      // _startTime = now; // remember starting time
    } // if

  } else if (_state == STATE_HIGH2) { // waiting for menu pin being released finally.
    if (!_inputLevel) {
      // this was a 2 click sequence.
      _send(ACTION_DOUBLECLICK);
      _state = STATE_INIT;
    } // if

  } else if (_state == STATE_PRESSHIGH) {
    // waiting for menu pin being release after long press.
    if (!_inputLevel) {
      _state = STATE_INIT;
    } // if
  } // if

} // loop()


void ButtonElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
} // pushState()


void ButtonElement::_send(ACTIONS action)
{
  String a;
  const char *actionName = NULL;

  if (action == ACTION_CLICK) {
    a = _clickAction;
    actionName = "click";

  } else if (action == ACTION_DOUBLECLICK) {
    a = _doubleClickAction;
    actionName = "doubleclick";

  } else if (action == ACTION_PRESS) {
    a = _pressAction;
    actionName = "press";
  }

  if (a.length() > 0)
    _board->dispatch(a);
  if (actionName)
    _board->dispatch(_actionAction, actionName);
} // _send()

// End
