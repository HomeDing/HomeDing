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
#include <HomeDing.h>

#include <ReferenceElement.h>

#define TRACE(...) // LOGGER_ETRACE(__VA_ARGS__)


/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new ReferenceElement
 * @return ReferenceElement* created element
 */
Element *ReferenceElement::create() {
  return (new ReferenceElement());
} // create()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool ReferenceElement::set(const char *name, const char *value) {
  bool ret = true;

  TRACE("set %s=%s", name, value);

  if (_stricmp(name, "value") == 0) {
    gotNewValue = true;
    _incomingValue = atof(value);
    TRACE("new value =%f", _incomingValue);

  } else if (_stricmp(name, "reference") == 0) {
    gotNewValue = true;
    _refValue = atof(value);
    TRACE("new ref =%f", _refValue);

  } else if (_stricmp(name, "invert") == 0) {
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
 * @brief Send out actions when new value is given.
 */
void ReferenceElement::loop() {
  if (gotNewValue) {
    // compare against reference and send reference actions
    _value = (_incomingValue > _refValue ? 1 : 0);

    if (_inverse) {
      _value = 1 - _value;
    }

    TRACE("%f/%f => %d", _incomingValue, _refValue, _value);
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
    std::function<void(const char *pName, const char *eValue)> callback) {
  Element::pushState(callback);
  callback("value", (_value ? "1" : "0"));
  callback("reference", String(_refValue).c_str());
} // pushState()


// End
