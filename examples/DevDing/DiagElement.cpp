/**
 * @file DiagElement.cpp
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
 * Changelog:see DiagElement.h
 */

#include <Arduino.h>
#include <Board.h>
#include <Element.h>

#include <HomeDing.h>

#include "DiagElement.h"


/* ===== Define local constants and often used strings ===== */

// like:
// #define ANYCONSTANT 1000


/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new DiagElement
 * @return DiagElement* created element
 */
Element *DiagElement::create()
{
  return (new DiagElement());
} // create()


/* ===== Element functions ===== */

DiagElement::DiagElement() {
  startupMode = Element_StartupMode::System;
}


// maybe: overwrite the init() function.

void DiagElement::init(Board *board)
{
  Element::init(board);

  // do something here like initialization
  startupMode = Element_StartupMode::System;
} // init()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool DiagElement::set(const char *name, const char *value)
{
  bool ret = true;

  if (_stricmp(name, PROP_VALUE) == 0) {
    _value = _atoi(value);

    // } else if (_stricmp(name, "onChange") == 0) {
    // save the actions
    // _changeAction = value;

    // } else if (_stricmp(name, "doAction") == 0) {
    // make something

  } else {
    ret = Element::set(name, value);
  } // if

  return (ret);
} // set()


/**
 * @brief Activate the DelementiagElement.
 */
void DiagElement::start()
{
  LOGGER_ETRACE("start()");
  Element::start();

  LOGGER_EINFO("Reset Reason: %s", ESP.getResetReason().c_str());
  LOGGER_EINFO(" Free Memory: %d", ESP.getFreeHeap());


  // ===== scan the the I2C bus and report found devices =====
  LOGGER_EINFO("Scan i2c (sda=%d, scl=%d)...", _board->I2cSda, _board->I2cScl);

  uint8_t error, adr;
  int num;

  num = 0;
  for (adr = 1; adr < 127; adr++) {
    // The i2c scanner uses the return value of Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(adr);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.printf(" 0x%02x found.\n", adr);
      num++;
    } else if (error == 4) {
      Serial.printf(" 0x%02x error.\n", adr);
    } // if
    yield();
  } // for
  Serial.printf(" %d devices found.\n", num);

  // ===== system indicators
  LOGGER_EINFO("Free Memory: %d", ESP.getFreeHeap());
} // start()


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void DiagElement::loop()
{
  // do something
} // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void DiagElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
  callback(PROP_VALUE, String(_value).c_str());
} // pushState()


// maybe: overwrite the term() function,
// void Element::term()
// {
//   LOGGER_ETRACE("term()");
//   active = false;
// } // term()


/* ===== Register the Element ===== */

// As long as the Element is project specific or is a element always used
// the registration is placed here without using a register #define.

// When transferred to the HomeDing library a #define like the
// HOMEDING_INCLUDE_XXX should be used to allow the sketch to select the
// available Elements. See <HomeDing.h> the move these lines to DiagElement.h:

// #ifdef HOMEDING_REGISTER
// Register the DiagElement onto the ElementRegistry.
bool DiagElement::registered =
    ElementRegistry::registerElement("diag", DiagElement::create);
// #endif

// End
