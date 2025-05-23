/**
 * @file SwitchElement.cpp
 * @brief Element switching a digital signal HIGH LOW based on actions.
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license,
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino.
 *
 * Changelog: see SwitchElement.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <SwitchElement.h>

/**
 * @brief static factory function to create a new SwitchElement.
 * @return SwitchElement* as Element* created element
 */
Element *SwitchElement::create()
{
  return (new SwitchElement());
} // create()


bool SwitchElement::_setValue(int val, bool forceAction)
{
  bool hasChanged = ValueElement::_setValue(val, forceAction);

  if (hasChanged) {
    if (val) {
      HomeDing::Actions::push(_highAction, _value);

    } else {
      HomeDing::Actions::push(_lowAction, _value);
    } // if
  } // if
  return (hasChanged);
} // _setValue()


void SwitchElement::init(Board *board)
{
  ValueElement::init(board);

  // The Switch is like a value element with the range 0..1
  _minRange = 0;
  _maxRange = 1;
}

/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool SwitchElement::set(const char *name, const char *value)
{
  bool ret = true;

  if (name == HomeDing::Actions::Value) {
    _setValue(_atob(value), false);

  } else if (name == HomeDing::Actions::Toggle) {
    _setValue(!_getValueInt(), false);

  } else if (name == HomeDing::Actions::OnHigh) {
    _highAction = value;

  } else if (name == HomeDing::Actions::OnLow) {
    _lowAction = value;

  } else {
    ret = ValueElement::set(name, value);
  } // if
  return (ret);
} // set()

// End
