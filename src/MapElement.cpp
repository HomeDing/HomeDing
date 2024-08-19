/**
 * @file MapElement.cpp
 * @brief Map incoming values to other values specified by a set of range rules.
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
 * Changelog:see MapElement.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include "MapElement.h"
#include "MicroJsonParser.h"

#if !defined(TRACE)
#define TRACE(...)  // LOGGER_ETRACE(__VA_ARGS__)
#endif

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new MapElement
 * @return MapElement* created element
 */
Element *MapElement::create() {
  return (new MapElement());
}  // create()


/* ===== Element functions ===== */

// MapElement::MapElement() {}


void MapElement::init(Board *board) {
  Element::init(board);
}  // init()


void MapElement::_mapValue(const char *value) {
  TRACE("map '%s'", value);
  int mapSize = _mMax.size();
  int cFound = -1;

  // walk through the max values to find the last one that fits.
  for (int c = 0; c < mapSize; c++) {
    bool ruleFits = true;  // until we find it is not

    // value lower than `min` ?
    String v = _mMin[c];
    if (v.isEmpty()) {
      // value is always inside when no lower boundary is given
    } else if ((_isStringType) && _stricmp(value, v.c_str()) < 0) {
      ruleFits = false;  // no match, value is too low
    } else if ((!_isStringType) && _atoi(value) < v.toInt()) {
      ruleFits = false;  // no match, value is too low
    }

    v = _mMax[c];
    // value higher than `max` ?
    if (v.isEmpty()) {
      // value is always inside when no upper boundary is given
    } else if ((_isStringType) && _stricmp(value, v.c_str()) > 0) {
      ruleFits = false;
    } else if ((!_isStringType) && _atoi(value) > v.toInt()) {
      ruleFits = false;
    }

    if (ruleFits) {
      cFound = c;
      break;
    }
  }  // for

  // TRACE("rule=%d", cFound);

  if (cFound < 0) {
    LOGGER_EERR("NO RULE FITS: '%s'", value);

  } else if ((cFound != _currentMapIndex) || (_resend)) {
    String v = _mValue[cFound];
    if (v.isEmpty()) {
      _value = value;
    } else {
      _value = v;
    }
    TRACE("new Value=%s", _value.c_str());
    _currentMapIndex = cFound;
    _needUpdate = true;
  }
}  // _mapValue()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool MapElement::set(const char *name, const char *value) {
  TRACE("set '%s'='%s'", name, value);
  bool ret = true;

  if (name == HomeDing::Action::Value) {
    // find the right map entry (first that matches)
    _mapValue(value);

  } else if (_stristartswith(name, "rules[")) {
    size_t i;
    String iName;
    _scanIndexParam(name, i, iName);
    TRACE(" iName:%s", iName.c_str());
    TRACE(" i:%d", i);

    // save all values and actions in the vectors.
    size_t mapIndex = _atoi(name + 6);  // number starts after "rules["
    char *mapName = strrchr(name, MICROJSON_PATH_SEPARATOR) + 1;

    // LOGGER_EINFO("map[%d] '%s'='%s'", mapIndex, mapName, value);

    if (_stricmp(mapName, "min") == 0) {
      _mMin.setAt(mapIndex, value);

    } else if (_stricmp(mapName, "max") == 0) {
      _mMax.setAt(mapIndex, value);

    } else if (_stricmp(mapName, "equal") == 0) {
      _mMin.setAt(mapIndex, value);
      _mMax.setAt(mapIndex, value);

    } else if (_stricmp(mapName, HomeDing::Action::Value) == 0) {
      _mValue.setAt(mapIndex, value);

    } else if (_stricmp(mapName, "onValue") == 0) {
      _mActions.setAt(mapIndex, value);

    }  // if

  } else if (name == HomeDing::Action::Type) {
    if (_stricmp(value, "string") == 0)
      _isStringType = true;

  } else if (name == HomeDing::Action::OnValue) {
    _valueAction = value;

  } else if (_stricmp(name, "resend") == 0) {
    _resend = _atob(value);

  } else {
    ret = Element::set(name, value);
  }  // if

  return (ret);
}  // set()


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void MapElement::loop() {
  if (_needUpdate) {
    _board->dispatch(_mActions[_currentMapIndex], _value);
    _board->dispatch(_valueAction, _value);
    _needUpdate = false;
  }
}  // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void MapElement::pushState(
  std::function<void(const char *pName, const char *eValue)> callback) {
  Element::pushState(callback);
  callback(HomeDing::Action::Value, _value.c_str());
}  // pushState()

// End
