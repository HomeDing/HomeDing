/**
 * @file MenuElement.cpp
 * @brief Menu Element implementation.
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
 * Changelog:see MenuElement.h
 */

#include <Arduino.h>
#include <Board.h>
#include <Element.h>

#include "ValueElement.h"
#include "MenuElement.h"
#include <ElementRegistry.h>


/* ===== Define local constants and often used strings ===== */

// like:
// #define ANYCONSTANT 1000

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new MenuElement
 * @return MenuElement* created element
 */
Element *MenuElement::create()
{
  return (new MenuElement());
} // create()


/* ===== Element functions ===== */

// http://lcddevice/$board/menu/0?value=1
// http://lcddevice/$board/menu/0?value=-1

// http://lcddevice/$board/menu/0?select=1
// http://lcddevice/$board/rotary/0?onvalue=menu/0?value=$v


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool MenuElement::set(const char *name, const char *value)
{
  LOGGER_ETRACE("set(%s, %s)", name, value);
  bool ret = true;

  if (_stricmp(name, "value") == 0) {
    if (_count > 0) {
      // LOGGER_RAW("_active:%d", _active);
      // LOGGER_RAW("valueList[]:%ld", valueList[_active]);
      valueList[_active]->set("up", value);
      _updateV = true;
    }

  } else if (_stricmp(name, "select") == 0) {
    // switch to next value in list..
    if (_count > 0) {
      _active = (_active + 1) % _count; // 2 value elements in list
      _updateM = true;
      _updateV = true;
    }

  } else if (_stricmp(name, "valueElements") == 0) {
    String s = value;
    s.concat(',');

    while (s.length() > 0) {
      // extract first name
      int pos = s.indexOf(',');
      String n = s.substring(0, pos);
      ValueElement *v = (ValueElement *)_board->getElement("value", n.c_str());
      if ((v) && _count < MAXMENUVALUES) {
        LOGGER_RAW("** ADD %s", n.c_str());
        valueList[_count++] = v;
      }
      s.remove(0, pos+1);
    }

  } else if (_stricmp(name, "onDisplay") == 0) {
    // save the actions
    _displayAction = value;

  } else if (_stricmp(name, "onValue") == 0) {
    // save the actions
    _valueAction = value;

  } else if (_stricmp(name, "onMenu") == 0) {
    // save the actions
    _menuAction = value;

  } else {
    ret = Element::set(name, value);
  } // if

  return (ret);
} // set()


/**
 * @brief Activate the MenuElement.
 */
void MenuElement::start()
{
  LOGGER_ETRACE("start()");

  // Verify parameters

  // if (parameters ok) {
  Element::start();

  // valueList[0] = (ValueElement *)_board->getElement("value", "volume");
  // valueList[1] = (ValueElement *)_board->getElement("value", "frequency");

  // LOGGER_ETRACE("start: %ld %ld", valueList[0], valueList[1]);

  // } // if

} // start()


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void MenuElement::loop()
{
  if ((_updateV) && _valueAction.length() > 0) {
    _board->dispatch(_valueAction, valueList[_active]->getValue());
  }

  if ((_updateM) && _menuAction.length() > 0) {
    LOGGER_RAW("do menuAction...");

    _board->dispatch(_menuAction, valueList[_active]->getValue());
  }

  if ((_updateM || _updateV) && _displayAction.length() > 0) {
    String v = valueList[_active]->getLabel();
    v.concat(":");
    v.concat(valueList[_active]->getValue());
    _board->dispatch(_displayAction, v);
  }

  _updateV = false;
  _updateM = false;
} // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void MenuElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
  callback("value", String(_value).c_str());
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
// HOMEDING_INCLUDE_My should be used to allow the sketch to select the
// available Elements. See <HomeDing.h> the move these lines to MenuElement.h:

// #ifdef HOMEDING_REGISTER
// Register the MenuElement onto the ElementRegistry.
bool MenuElement::registered =
    ElementRegistry::registerElement("menu", MenuElement::create);
// #endif

// End
