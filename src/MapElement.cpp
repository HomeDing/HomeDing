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

#define TRACE(...) // LOGGER_ETRACE(__VA_ARGS__)

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new MapElement
 * @return MapElement* created element
 */
Element *MapElement::create() {
  return (new MapElement());
} // create()


/* ===== Element functions ===== */

// MapElement::MapElement() {}


void MapElement::init(Board *board) {
  Element::init(board);

  // create the map members with a reasonable size.
  this->_mMin.reserve(2);
  this->_mMax.reserve(2);
  this->_mValue.reserve(2);
  this->_mActions.reserve(2);
} // init()


void MapElement::_mapValue(const char *value) {
  TRACE("map '%s'", value);
  int mapSize = _mMax.size();
  int cFound = -1;

  // walk through the max values to find the last one that fits.
  for (int c = 0; c < mapSize; c++) {
    bool ruleFits = true; // until we find it is not

    // value lower than `min` ?
    if (_mMin[c].isEmpty()) {
      // value is always inside when no lower boundary is given
    } else if ((_isStringType) && _stricmp(value, _mMin[c].c_str()) < 0) {
      ruleFits = false; // no match, value is too low
    } else if ((!_isStringType) && _atoi(value) < _mMin[c].toInt()) {
      ruleFits = false; // no match, value is too low
    }

    // value higher than `max` ?
    if (_mMax[c].isEmpty()) {
      // value is always inside when no upper boundary is given
    } else if ((_isStringType) && _stricmp(value, _mMax[c].c_str()) > 0) {
      ruleFits = false;
    } else if ((!_isStringType) && _atoi(value) > _mMax[c].toInt()) {
      ruleFits = false;
    }

    if (ruleFits) {
      cFound = c;
      break;
    }
  } // for

  // TRACE("rule=%d", cFound);

  if (cFound < 0) {
    LOGGER_EERR("NO RULE FITS: '%s'", value);
  } else if (cFound != _currentMapIndex) {
    if (_mValue[cFound].isEmpty()) {
      _value = value;
    } else {
      _value = _mValue[cFound];
    }
    TRACE("new Value=%s", _value.c_str());
    _currentMapIndex = cFound;
    _needUpdate = true;
  }
} // _mapValue()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool MapElement::set(const char *name, const char *value) {
  TRACE("set '%s'='%s'", name, value);
  bool ret = true;

  if (_stricmp(name, PROP_VALUE) == 0) {
    // find the right map entry (first that matches)
    _mapValue(value);

  } else if (_stricmp(name, "type") == 0) {
    if (_stricmp(value, "string") == 0)
      _isStringType = true;

  } else if (_stricmp(name, ACTION_ONVALUE) == 0) {
    _valueAction = value;

  } else if (_stristartswith(name, "rules[")) {
    // save all values and actions in the vectors.
    size_t mapIndex = _atoi(name + 6); // number starts after "rules["
    char *mapName = strrchr(name, MICROJSON_PATH_SEPARATOR) + 1;

    // LOGGER_EINFO("map[%d] '%s'='%s'", mapIndex, mapName, value);

    if (mapIndex >= _mMax.size()) {
      // request space for new index
      _mMin.resize(mapIndex + 2);
      _mMax.resize(mapIndex + 2);
      _mValue.resize(mapIndex + 2);
      _mActions.resize(mapIndex + 2);
    } // if

    if (_stricmp(mapName, "min") == 0) {
      _mMin[mapIndex] = value;

    } else if (_stricmp(mapName, "max") == 0) {
      _mMax[mapIndex] = value;

    } else if (_stricmp(mapName, "equal") == 0) {
      _mMin[mapIndex] = value;
      _mMax[mapIndex] = value;

    } else if (_stricmp(mapName, PROP_VALUE) == 0) {
      _mValue[mapIndex] = value;

    } else if (_stricmp(mapName, ACTION_ONVALUE) == 0) {
      _mActions[mapIndex] = value;
    } // if

  } else {
    ret = Element::set(name, value);
  } // if

  return (ret);
} // set()


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void MapElement::loop() {
  if (_needUpdate) {
    _board->dispatch(_mActions[_currentMapIndex], _value);
    _board->dispatch(_valueAction, _value);
    _needUpdate = false;
  }
} // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void MapElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback) {
  Element::pushState(callback);
  callback(PROP_VALUE, _value.c_str());
} // pushState()

// End
