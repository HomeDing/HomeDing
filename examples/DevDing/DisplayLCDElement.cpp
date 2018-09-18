/**
 * @file DisplayLCDElement.cpp
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
 * Changelog:see DisplayLCDElement.h
 */


#include "DisplayLCDElement.h"
#include "ElementRegistry.h"

#define LOGGER_MODULE "lcd"
#include "core/Logger.h"

#include "DisplayAdapterLCD.h"

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new DisplayLCDElement
 * @return DisplayLCDElement* created element
 */
Element *DisplayLCDElement::create()
{
  LOGGER_TRACE("create()");
  return (new DisplayLCDElement());
} // create()


/* ===== Element functions ===== */

/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool DisplayLCDElement::set(const char *name, const char *value)
{
  LOGGER_TRACE("set(%s, %s)", name, value);
  bool ret = true;

  if (_stricmp(name, "address") == 0) {
    _address = atoi(value);

  } else if (_stricmp(name, "sda") == 0) {
    _sda = atoi(value);

  } else if (_stricmp(name, "scl") == 0) {
    _scl = atoi(value);

  } else {
    ret = Element::set(name, value);
  } // if

  return (ret);
} // set()


/**
 * @brief Activate the DisplayLCDElement.
 */
void DisplayLCDElement::start()
{
  DisplayAdapter *d;
  LOGGER_TRACE("start(0x%x,%d,%d)", _address, _sda, _scl);
  d = (DisplayAdapter *)(new DisplayAdapterLCD(_address, _sda, _scl));
  _board->display = d;
  d->init();
  LOGGER_TRACE("display=0x%x", _board->display);

  Element::start();
} // start()


/* ===== Register the Element ===== */

// As long as the Element is project specific or is a element always used
// the registration is placed here without using a register #define.

// When transferred to the HomeDing library a #define like the
// HOMEDING_INCLUDE_My should be used to allow the sketch to select the
// available Elements. See <HomeDing.h> the move these lines to DisplayLCDElement.h:

// #ifdef HOMEDING_REGISTER
// Register the DisplayLCDElement onto the ElementRegistry.
bool DisplayLCDElement::registered =
    ElementRegistry::registerElement("displaylcd", DisplayLCDElement::create);
// #endif

// End
