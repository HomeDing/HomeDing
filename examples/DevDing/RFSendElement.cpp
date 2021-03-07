/**
 * @file RFSendElement.cpp
 * @brief Element Template class.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license.
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:see RFSendElement.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include "RFSendElement.h"

// #define NODEBUG

#include <TabRF.h>
#include <intertechno_protocol.h> // use the intertechno2 code defintions


/* ===== Define local constants and often used strings ===== */


/* ===== Define static class variables here. ====== */

SignalParser sig;
TabRFClass tabRF;

int RFSendElement::_rfpin = -1;

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new RFSendElement
 * @return RFSendElement* created element
 */
Element *RFSendElement::create()
{
  return (new RFSendElement());
} // create()


/* ===== Element functions ===== */

/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool RFSendElement::set(const char *name, const char *value)
{
  bool ret = true;

  if (_stricmp(name, PROP_VALUE) == 0) {
    _lastValue = _atoi(value);

    if (active) {
      if (_lastValue) {
        // TRACE("send(%s)", _codeOn.c_str());
        tabRF.send(_codeOn.c_str());
      } else {
        // TRACE("send(%s)", _codeOff.c_str());
        tabRF.send(_codeOff.c_str());
      }
    } // if

  } else if (_stricmp(name, PROP_PIN) == 0) {
    _rfpin = _atopin(value);

  } else if (_stricmp(name, "codeOn") == 0) {
    _codeOn = value;

  } else if (_stricmp(name, "codeOff") == 0) {
    _codeOff = value;

  } else {
    ret = Element::set(name, value);
  } // if

  return (ret);
} // set()


/**
 * @brief Activate the RFSendElement.
 */
void RFSendElement::start()
{
  static bool initialized = false;
  // TRACE("start()");

  // initialize the tabrf library only once.
  if (!initialized) {
    // TRACE("start on pin=%d", _rfpin);

    tabRF.init(&sig, NO_PIN, _rfpin, 16); // input at D1 = pin #2 , output at D4, pin # 9

    register_intertechno1(sig);
    register_intertechno2(sig);

    initialized = true;
  }

  Element::start();
} // start()


/**
 * @brief push the current value of all properties to the callback.
 */
void RFSendElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
  callback(PROP_VALUE, (_lastValue == 1) ? "1" : "0");
} // pushState()


// End
