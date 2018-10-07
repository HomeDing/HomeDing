/**
 * @file MyElement.cpp
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
 * Changelog:see MyElement.h
 */


#include "MyElement.h"
#include "ElementRegistry.h"

#undef LOGGER_MODULE
#define LOGGER_MODULE "my"
#include "core/Logger.h"

/* ===== Define local constants and often used strings ===== */

// like:
// #define TIMER_UNIT 1000


/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new MyElement
 * @return MyElement* created element
 */
Element *MyElement::create()
{
  LOGGER_TRACE("create()");
  return (new MyElement());
} // create()


/* ===== Element functions ===== */


// maybe: overwrite the init() function.

// void MyElement::init(Board *board)
// {
//   LOGGER_TRACE("init()");
//   Element::init(board);
//   // do something here like initialization
// } // init()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool MyElement::set(const char *name, const char *value)
{
  LOGGER_TRACE("set(%s, %s)", name, value);
  bool ret = true;

  if (_stricmp(name, "value") == 0) {
    _value = atoi(value);

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
 * @brief Activate the MyElement.
 */
void MyElement::start()
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
void MyElement::loop()
{
  // do something
} // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void MyElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
  callback("value", String(_value).c_str());
} // pushState()


// maybe: overwrite the term() function,


/* ===== Register the Element ===== */

// As long as the Element is project specific or is a element always used
// the registration is placed here without using a register #define.

// When transferred to the HomeDing library a #define like the
// HOMEDING_INCLUDE_My should be used to allow the sketch to select the
// available Elements. See <HomeDing.h> the move these lines to MyElement.h:

// #ifdef HOMEDING_REGISTER
// Register the MyElement onto the ElementRegistry.
bool MyElement::registered =
    ElementRegistry::registerElement("my", MyElement::create);
// #endif

// End
