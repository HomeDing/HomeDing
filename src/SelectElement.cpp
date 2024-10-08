/**
 * @file SelectElement.cpp
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:see SelectElement.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include "SelectElement.h"

#if !defined(TRACE)
#define TRACE(...) // LOGGER_ETRACE(__VA_ARGS__)
#endif

// private functions

void SelectElement::_selectOption(int n) {
  TRACE("select(%d)", n);
  if (_selected != n) {
    HomeDing::Actions::push(_keyAction, _keys[n]);
    HomeDing::Actions::push(_valueAction, _values[n]);
    _selected = n;
  }
}


/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new SelectElement
 * @return SelectElement* created element
 */
Element *SelectElement::create() {
  return (new SelectElement());
}  // create()


/* ===== Element functions ===== */

bool SelectElement::set(const char *name, const char *value) {
  TRACE("set %s=%s", name, value);
  bool ret = true;
  uint16_t size = _keys.size();

  if (Element::set(name, value)) {
    // done.
  } else if (active && size) {
    if (_stricmp(name, "key") == 0) {
      for (int i = 0; i < size; i++) {
        if (_keys[i].equalsIgnoreCase(value)) {
          _selectOption(i);
          break;  // for
        }
      }

    } else if (_stricmp(name, "index") == 0) {
      int i = _atoi(value);
      if ((i >= 0) && (i < size)) {
        _selectOption(i);
      }

    } else if (_stricmp(name, "next") == 0) {
      // start next option
      if (_cycle || (_selected + 1 < size)) {
        _selectOption((_selected + 1) % size);
      }

    } else if (_stricmp(name, "prev") == 0) {
      // start previous option
      if (_cycle || (_selected > 0)) {
        _selectOption((_selected - 1) % size);
      }
    }

  } else if (_stricmp(name, "cycle") == 0) {
    _cycle = _atob(value);

  } else if (_stristartswith(name, "options[")) {
    size_t i;
    String iName;
    _scanIndexParam(name, i, iName);

    if (iName.equalsIgnoreCase("key")) {
      TRACE("key[%d]=%s", i, value);
      _keys.setAt(i, value);

    } else if (iName.equalsIgnoreCase(HomeDing::Action::Value)) {
      TRACE("value[%d]=%s", i, value);
      _values.setAt(i, value);
    }

  } else if (_stricmp(name, "onkey") == 0) {
    _keyAction = value;

  } else if (name == HomeDing::Action::OnValue) {
    _valueAction = value;

  } else {
    ret = false;
  }  // if

  return (ret);
}  // set()


/**
 * @brief Activate the SelectElement.
 */
void SelectElement::start() {
  TRACE("start()");

  // Verify parameters
  for (int n = 0; n < _values.size(); n++) {
    TRACE("option=%s:%s", _keys[n].c_str(), _values[n].c_str());
  }

  // if (parameters ok) {
  Element::start();
  // } // if

}  // start()


/**
 * @brief push the current value of all properties to the callback.
 */
void SelectElement::pushState(
  std::function<void(const char *pName, const char *eValue)> callback) {
  Element::pushState(callback);
  callback("index", _printInteger(_selected));
  callback("key", _keys[_selected].c_str());
  callback(HomeDing::Action::Value, _values[_selected].c_str());
}  // pushState()


// End
