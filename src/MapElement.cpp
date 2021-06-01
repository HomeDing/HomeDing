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


/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new MapElement
 * @return MapElement* created element
 */
Element *MapElement::create()
{
  return (new MapElement());
} // create()


/* ===== Element functions ===== */

// MapElement::MapElement() {}


void MapElement::init(Board *board)
{
  Element::init(board);

  // create the map members with a reasonable size.
  this->_mMax.reserve(4);
  this->_mValue.reserve(4);
  this->_mActions.reserve(4);
} // init()


void MapElement::_mapValue(const char *value)
{
  // LOGGER_ETRACE("map '%s'", value);
  int mapSize = _mMax.size();
  int cFound = 0;

  // walk through the max values to find the last one that fits.
  for (int c = 0; c < mapSize; c++) {
    if (_isStringType && (_stricmp(value, _mMax[c].c_str()) <= 0)) {
      cFound = c;
      break;

    } else if (_atoi(value) <= _mMax[c].toInt()) {
      cFound = c;
      break;
    } // if
  } // for

  if (cFound != _currentMapIndex) {
    // LOGGER_EINFO("cFound=%d", cFound);
    // LOGGER_EINFO("cValue=%s", _mValue[cFound].c_str());
    _value = _mValue[cFound]; // new value,
    _board->dispatch(_mActions[cFound], _value);
    _board->dispatch(_valueAction, _value);
  }
  _currentMapIndex = cFound;
} // _mapValue()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool MapElement::set(const char *name, const char *value)
{
  // LOGGER_ETRACE("set '%s'='%s'", name, value);
  bool ret = true;

  if (_stricmp(name, PROP_VALUE) == 0) {
    // find the right map entry (first that matches)
    _mapValue(value);

  } else if (_stricmp(name, "type") == 0) {
    if (_stricmp(value, "string") == 0)
      _isStringType = true;

  } else if (_stricmp(name, ACTION_ONVALUE) == 0) {
    _valueAction = value;

  } else if (_stristartswith(name, "maps[")) {
    // save all values and actions in the vectors.
    size_t mapIndex = _atoi(name + 5); // number starts after "maps["
    char *mapName = strrchr(name, MICROJSON_PATH_SEPARATOR) + 1;

    // LOGGER_EINFO("map[%d] '%s'='%s'", mapIndex, mapName, value);

    if (mapIndex >= _mMax.size()) {
      // set default values for new index
      _mMax.resize(mapIndex + 1);
      _mMax[mapIndex] = "";
      _mValue.resize(mapIndex + 1);
      _mValue[mapIndex] = "";
      _mActions.resize(mapIndex + 1);
      _mActions[mapIndex] = "";
    } // if

    if (_stricmp(mapName, "max") == 0) {
      _mMax[mapIndex] = value;

    } else if (_stricmp(mapName, PROP_VALUE) == 0) {
      _mValue[mapIndex] = value;

    } else if (_stricmp(mapName, "onmap") == 0) {
      _mActions[mapIndex] = value;
    } // if

  } else {
    ret = Element::set(name, value);
  } // if

  return (ret);
} // set()


/**
 * @brief Activate the MapElement.
 */
void MapElement::start()
{
  // LOGGER_ETRACE("start()");

  // // Debug output of parameters
  // int mapSize = _mMax.size();
  // LOGGER_EINFO("mapMax.size()=%d", mapSize);

  // for (int n = 0; n < mapSize; n++) {
  //   LOGGER_EINFO("map[%d]: %s %s -> %s", n, _mMax[n].c_str(), _mValue[n].c_str(), _mActions[n].c_str());
  // } // for

  Element::start();

} // start()


/**
 * @brief push the current value of all properties to the callback.
 */
void MapElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
  callback(PROP_VALUE, _value.c_str());
} // pushState()

// End
