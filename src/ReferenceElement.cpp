/**
 * @file ReferenceElement.cpp
 * @brief Element that compares the incoming value against a reference value
 * and sends action with value=1 when above the reference value otherwise value=0.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD 3-Clause style license,
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:see ReferenceElement.h
 */

#include <Arduino.h>
#include <Board.h>
#include <Element.h>

#include <HomeDing.h>

#include "ReferenceElement.h"


/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new ReferenceElement
 * @return ReferenceElement* created element
 */
Element *ReferenceElement::create()
{
  return (new ReferenceElement());
} // create()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool ReferenceElement::set(const char *name, const char *value)
{
  bool ret = true;

  if (_stricmp(name, PROP_VALUE) == 0) {
    gotNewValue = true;
    _incomingValue = atof(value);

  } else if (_stricmp(name, "reference") == 0) {
    gotNewValue = true;
    _refValue = atof(value);

  } else if (_stricmp(name, PROP_INVERSE) == 0) {
    _inverse = _atob(value);

  } else if (_stricmp(name, "onreference") == 0) {
    _referenceAction = value;

  } else if (_stricmp(name, "onhigh") == 0) {
    _highAction = value;

  } else if (_stricmp(name, "onlow") == 0) {
    _lowAction = value;

  } else {
    ret = Element::set(name, value);
  } // if

  return (ret);
} // set()


/**
 * @brief Activate the ReferenceElement.
 */
void ReferenceElement::start()
{
  if (_refValue != FLT_MIN) {
    Element::start();
  } // if
} // start()


/**
 * @brief Send out actions when new value is given.
 */
void ReferenceElement::loop()
{
  if (gotNewValue) {
    // compare against reference and send reference actions
    _value = (_incomingValue > _refValue ? 1 : 0);

    if (_inverse) {
      _value = 1 - _value;
    }
    _board->dispatch(_referenceAction, _value);

    if (_value) {
      _board->dispatch(_highAction);
    } else {
      _board->dispatch(_lowAction);
    } // if
    gotNewValue = false;
  }
} // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void ReferenceElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
  callback(PROP_VALUE, (_value ? "1" : "0"));
  callback("reference", String(_refValue).c_str());
} // pushState()


// End
