/**
 * @file SceneElement.cpp
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:see SceneElement.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include "SceneElement.h"

#define TRACE(...) LOGGER_ETRACE(__VA_ARGS__)

/* ===== Define local constants and often used strings ===== */

// like:
// #define ANYCONSTANT 1000


/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new SceneElement
 * @return SceneElement* created element
 */
Element *SceneElement::create()
{
  return (new SceneElement());
} // create()


/* ===== Element functions ===== */

SceneElement::SceneElement()
{
  // adjust startupMode when Network (default) is not applicable.
  // startupMode = Element_StartupMode::System;
  _value = "";
  _newValue = false;
}


void SceneElement::init(Board *board)
{
  TRACE("init()");
  Element::init(board);
  // do something here like initialization
} // init()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool SceneElement::set(const char *name, const char *value)
{
  bool ret = true;

  if (_stricmp(name, "value") == 0) {
    // A new value action triggers the scene element
    _value = value;
    _newValue = true;

  } else if (_stristartswith(name, "onvalue[")) {
    size_t index = _atoi(name + 8); // number starts after "onvalue["

    if (index >= _actions.size()) {
      // set default values for new index
      _actions.resize(index + 1);
      _actions[index] = value;
      TRACE("new(%d):<%s>", index, value);
    } // if

  } else {
    ret = Element::set(name, value);
  } // if

  return (ret);
} // set()


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void SceneElement::loop()
{
  if (_newValue) {
    // send out scene actions...
    int size = _actions.size();

    // walk through the max values to find the last one that fits.
    for (int c = 0; c < size; c++) {
      TRACE("send(%d):<%s>", c, _actions[c].c_str());

      _board->dispatch(_actions[c], _value);
    } // for
    _newValue = false;
  }
  // do something
} // loop()


/**
 * @brief push the current value to the callback.
 */
void SceneElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
  callback(PROP_VALUE, _value.c_str());
} // pushState()


void SceneElement::term()
{
  TRACE("term()");
  active = false;
} // term()


/* ===== Register the Element ===== */

// As long as the Element is project specific or is a element always used
// the registration is placed here without using a register #define.

// When transferred to the HomeDing library a #define like the
// HOMEDING_INCLUDE_XXX should be used to allow the sketch to select the
// available Elements. See <HomeDing.h> the move these lines to SceneElement.h:

// #ifdef HOMEDING_REGISTER
// Register the SceneElement onto the ElementRegistry.
bool SceneElement::registered =
    ElementRegistry::registerElement("scene", SceneElement::create);
// #endif

// End
