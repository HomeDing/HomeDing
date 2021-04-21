/**
 * @file RTCStateElement.cpp
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 *
 * @brief Element Template class.
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:see RTCStateElement.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include "RTCStateElement.h"

#define TRACE(...) // LOGGER_ETRACE(__VA_ARGS__)


/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new RTCStateElement
 * @return RTCStateElement* created element
 */
Element *RTCStateElement::create()
{
  return (new RTCStateElement());
} // create()


/* ===== Element functions ===== */

void RTCStateElement::init(Board *board)
{
  TRACE("init()");
  Element::init(board);
  _hasChanged = false;
  _board->state = this;
} // init()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool RTCStateElement::set(const char *name, const char *value)
{
  bool ret = true;

  if (_stricmp(name, "anyconfigProp") == 0) {
    // save for later use

  } else {
    ret = Element::set(name, value);
  } // if

  return (ret);
} // set()


/* ===== State Element functions ===== */

/**
 * @brief The save function allows saving state information specific for an Element.
 */
void RTCStateElement::save(Element *e, char const *key, char const *value)
{
  TRACE("save(%s=%s)", key, value);

  String entry = String(e->id) + "?" + key + "=";

  int n = _stateList.indexOf(entry);
  entry.concat(value);

  if (n < 0) {
    // add to the end
    if (_stateList.length() > 0)
      _stateList.concat(VALUE_SEPARATOR);
    _stateList.concat(entry);

  } else {
    String tmp;

    if (n > 0) {
      tmp.concat(_stateList.substring(0, n)); // all actions before incl. VALUE_SEPARATOR
    }
    tmp.concat(entry);

    int e = _stateList.indexOf(VALUE_SEPARATOR, n);
    if (e >= 0) {
      tmp.concat(_stateList.substring(e));
    }
    _stateList = tmp;
  } // if
  _hasChanged = true;
};


/**
 * @brief Load all state information from RTC memory and dispatch them as actions.
 */
void RTCStateElement::load()
{
  TRACE("load()");
  String loadState;

  loadState = RTCVariables::getStateString();
  while (loadState.length() > 0) {
    String a = popItemValue(loadState);
    _board->dispatchAction(a);
  } // while
};


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void RTCStateElement::loop()
{
  Element::loop();
  if (_hasChanged) {
    TRACE("save (%s)", _stateList.c_str());
    RTCVariables::setStateString(_stateList);
    _hasChanged = false;
  }
} // loop()


/* ===== Register the Element ===== */

// As long as the Element is project specific or is a element always used
// the registration is placed here without using a register #define.

// When transferred to the HomeDing library a #define like the
// HOMEDING_INCLUDE_XXX should be used to allow the sketch to select the
// available Elements. See <HomeDing.h> the move these lines to RTCStateElement.h:

// #ifdef HOMEDING_REGISTER
// Register the RTCStateElement onto the ElementRegistry.
bool RTCStateElement::registered =
    ElementRegistry::registerElement("rtcstate", RTCStateElement::create);
// #endif

// End
