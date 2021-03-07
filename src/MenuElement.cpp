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
#include <HomeDing.h>

#include <MenuElement.h>


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
      _updateM = _updateV = true;
    }

  } else if (_stricmp(name, "valueElements") == 0) {
    int n = 0;
    while (1) {
      String name = getItemValue(value, n++);
      if (name.length() > 0) {
        ValueElement *v = static_cast<ValueElement *>(_board->getElement("value", name.c_str()));
        if (!v)
          v = static_cast<ValueElement *>(_board->getElement("switch", name.c_str()));
        if ((v) && _count < MAXMENUVALUES) {
          valueList[_count++] = v;
        }
      } else {
        break;
      }
    }

  } else if (_stricmp(name, "onDisplay") == 0) {
    _displayAction = value;

  } else if (_stricmp(name, ACTION_ONVALUE) == 0) {
    _valueAction = value;

  } else if (_stricmp(name, "onMenu") == 0) {
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
  if (_updateM) {
    _board->dispatch(_menuAction, valueList[_active]->getLabel());
  }

  if (_updateV) {
    _board->dispatch(_valueAction, valueList[_active]->getValue());
  }

  if (_updateM || _updateV) {
    String v = valueList[_active]->getLabel();
    v.concat(":");
    v.concat(valueList[_active]->getValue());
    _board->dispatch(_displayAction, v);
  }

  _updateM =_updateV = false;
} // loop()

// End
