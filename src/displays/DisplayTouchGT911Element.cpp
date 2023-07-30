/**
 * @file DisplayTouchGT911Element.cpp
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:see DisplayTouchGT911Element.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <displays/DisplayButtonElement.h>

#include "DisplayTouchGT911Element.h"

#include <Wire.h>


#define TRACE(...)  // LOGGER_ETRACE(__VA_ARGS__)

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new DisplayTouchGT911Element
 * @return DisplayTouchGT911Element* created element
 */
Element *DisplayTouchGT911Element::create() {
  return (new DisplayTouchGT911Element());
}  // create()


/* ===== Private functions ===== */

/* ===== Element functions ===== */

DisplayTouchGT911Element::DisplayTouchGT911Element() {
  // adjust startupMode when Network (default) is not applicable.
  // startupMode = Element_StartupMode::System;
}


void DisplayTouchGT911Element::init(Board *board) {
  TRACE("init()");
  Element::init(board);
  // do something here like initialization
}  // init()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool DisplayTouchGT911Element::set(const char *name, const char *value) {
  bool ret = true;

  if (Element::set(name, value)) {
    // done.

  } else if (_stricmp(name, "address") == 0) {
    _address = _atoi(value);

  } else if (_stricmp(name, "width") == 0) {
    _width = _atoi(value);

  } else if (_stricmp(name, "height") == 0) {
    _height = _atoi(value);

  } else if (_stricmp(name, "rotation") == 0) {
    _rotation = _atoi(value);

  } else if (_stricmp(name, "interruptpin") == 0) {
    _interruptPin = _atopin(value);

  } else if (_stricmp(name, "resetpin") == 0) {
    _resetPin = _atopin(value);

    // } else if (_stricmp(name, "onvalue") == 0) {
    // save the actions
    // _xAction = value;

    // } else if (_stricmp(name, "doAction") == 0) {
    // make something

  } else {
    ret = false;
  }  // if

  return (ret);
}  // set()


/**
 * @brief Activate the DisplayTouchGT911Element.
 */
void DisplayTouchGT911Element::start() {
  TRACE("DisplayTouchGT911Element::start()");

  // Verify parameters

  // if (parameters ok) {
  Element::start();

  // // set the digital pin as output:
  tp = new GT911(_interruptPin, _resetPin, _width, _height);

  tp->init(false);
  tp->setRotation(_rotation);
  nextRead = millis() + 50;
  _bFound = nullptr;

  // } // if
}  // start()


/// @brief Poll the touch controller for touch points
void DisplayTouchGT911Element::loop() {
  Element::loop();
  unsigned long now = millis();

  if (now > nextRead) {
    GDTpoint_t points[GT911_MAX_CONTACTS];
    uint8_t count = tp->getTouchPoints(points);

    if (count > 0) {
      // as of now only interested in the first.
      TRACE("-touch %d/%d\n", points[0].x, points[0].y);
      lastX = points[0].x;
      lastY = points[0].y;

      if (!_bFound) {
        // find displaybutton at x/y
        _board->forEach("displaybutton", [this](Element *e) {
          if (!_bFound) {
            DisplayButtonElement *be = (DisplayButtonElement *)e;
            if (be->touchStart(lastX, lastY)) {
              _bFound = be;
            }
          }
        });

      } else {
        bool over = _bFound->touchStart(lastX, lastY);
        if (!over) {
          _bFound = nullptr;
        }
      }

    } else if (_bFound) {
      TRACE("-end");
      // call touchEnd to found button
      _bFound->touchEnd(lastX, lastY);
      _bFound = nullptr;

    }  // if

    nextRead = millis() + 50;
  }
}  // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void DisplayTouchGT911Element::pushState(
  std::function<void(const char *pName, const char *eValue)> callback) {
  Element::pushState(callback);
  // callback(PROP_VALUE, _printInteger(_value));
}  // pushState()


void DisplayTouchGT911Element::term() {
  TRACE("term()");
  Element::term();
}  // term()


// End
