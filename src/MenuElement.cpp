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

#include "MenuElement.h"


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

/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool MenuElement::set(const char *name, const char *value)
{
  bool ret = true;

  if (_stricmp(name, PROP_VALUE) == 0) {
    if (_count > 0) {
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
 * @brief Give some processing time to the Element to check for next actions.
 */
void MenuElement::loop()
{
  if ((_updateV) && _valueAction.length() > 0) {
    _board->dispatch(_valueAction, valueList[_active]->getValue());
  }

  if ((_updateM) && _menuAction.length() > 0) {
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

// End
