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


#include "ElementRegistry.h"
#include "RFSendElement.h"

#define LOGGER_MODULE "RFSend"
#include "Logger.h"

/* ===== Define local constants and often used strings ===== */

// like:
// #define TIMER_UNIT 1000


/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new RFSendElement
 * @return RFSendElement* created element
 */
Element *RFSendElement::create()
{
  LOGGER_TRACE("create()");
  return (new RFSendElement());
} // create()


/* ===== Element functions ===== */


// maybe: overwrite the init() function.


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool RFSendElement::set(const char *name, const char *value)
{
  LOGGER_TRACE("set(%s, %s)", name, value);
  bool ret = true;

  if (_stricmp(name, "value") == 0) {
    _lastValue = atoi(value);

    if (active) {
      if (_lastValue)
        tabRF.send(_codeOn.c_str());
      else
        tabRF.send(_codeOff.c_str());
    } // if

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
  LOGGER_TRACE("start()");

  // Verify parameters

  // if (parameters ok) {
  Element::start();
  // } // if

} // start()


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void RFSendElement::loop()
{
  // do something
} // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void RFSendElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
  callback("value", (_lastValue == 1) ? "1" : "0");
} // pushState()


// maybe: overwrite the term() function,


/* ===== Register the Element ===== */

// As long as the Element is project specific or is a element always used
// the registration is placed here without using a register #define.

// When transferred to the HomeDing library a #define like the
// HOMEDING_INCLUDE_RFSEND should be used to allow the sketch to select the
// available Elements. See <HomeDing.h> the move these lines to
// RFSendElement.h

#ifdef HOMEDING_REGISTER
// Register the RFSendElement onto the ElementRegistry.
bool RFSendElement::registered =
    ElementRegistry::registerElement("rfsend", RFSendElement::create);
#endif

// End
