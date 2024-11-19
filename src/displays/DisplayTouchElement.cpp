/**
 * @file DisplayTouchElement.cpp
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:see DisplayTouchElement.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#if defined(ESP32)

#include <displays/DisplayButtonElement.h>

#include "DisplayTouchElement.h"

#include <Wire.h>


#define TRACE(...) // LOGGER_ETRACE(__VA_ARGS__)

/* ===== Private functions ===== */

/* ===== Element functions ===== */

DisplayTouchElement::DisplayTouchElement() {
  // adjust startupMode when Network (default) is not applicable.
  // startupMode = Element::STARTUPMODE::System;
}

bool DisplayTouchElement::pullSensorData() {
  return (false);
}


void DisplayTouchElement::init(Board *board) {
  TRACE("init()");
  Element::init(board);
  // do something here like initialization
}  // init()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool DisplayTouchElement::set(const char *name, const char *value) {
  TRACE("set(%s, %s)", name, value);
  bool ret = true;

  if (Element::set(name, value)) {
    // done.

  } else if (name == HomeDing::Actions::Address) {
    _address = _atoi(value);

  } else if (name == HomeDing::Actions::Width) {
    _width = _atoi(value);

  } else if (name == HomeDing::Actions::Height) {
    _height = _atoi(value);

  } else if (_stricmp(name, "rotation") == 0) {
    _rotation = _atoi(value);

  } else if (_stricmp(name, "interruptpin") == 0) {
    _interruptPin = _atopin(value);

  } else if (_stricmp(name, "resetpin") == 0) {
    _resetPin = _atopin(value);

  } else if (_stricmp(name, "ontouch") == 0) {
    _touchAction = value;

  } else {
    ret = false;
  }  // if

  return (ret);
}  // set()


/// @brief setup the element so it can be started and stopped.
void DisplayTouchElement::setup() {
  TRACE("setup()");

  if (!_width) {
    _width = HomeDing::displayConfig.width;
  }
  if (!_height) {
    _height = HomeDing::displayConfig.height;
  }
  TRACE(" _width=%d _height=%d", _width, _height);
}


/**
 * @brief Activate the DisplayTouchElement.
 */
void DisplayTouchElement::start() {
  TRACE("DisplayTouchElement::start()");

  Element::start();
  nextRead = millis() + 50;
  _bFound = nullptr;
  _isTouched = false;
}  // start()


/// @brief Poll the touch controller for touch points
void DisplayTouchElement::loop() {
  Element::loop();
  unsigned long now = millis();

  if (now > nextRead) {

    if (pullSensorData()) {
      // got lastX and lastY

      // rotate
      if (_rotation == 0) {
        // no turn required

      } else if (_rotation == 90) {
        // 90° to the right
        int16_t temp = lastX;
        lastX = lastY;
        lastY = _width - temp;

      } else if (_rotation == 180) {
        // 180°
        lastX = _width - lastX;
        lastY = _height - lastY;

      } else if (_rotation == 270) {
        // 90° to the left
        int16_t temp = lastX;
        lastX = _height - lastY;
        lastY = temp;
      }

      if (!_isTouched) {
        HomeDing::Actions::push(_touchAction);
      }
      _isTouched = true;

      // as of now only interested in the first.
      TRACE(" touch %d/%d", lastX, lastY);
      if (!_bFound) {
        // find displaybutton at x/y
        _board->forEach(CATEGORY::Widget, [this](Element *e) {
          if ((!_bFound) && (Element::_stristartswith(e->id, "displaybutton/"))) {
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
          _isTouched = false;
        }
      }

    } else {
      if (_bFound) {
        TRACE("-end");
        // call touchEnd to found button
        _bFound->touchEnd(lastX, lastY);
        _bFound = nullptr;
      }  // if
      _isTouched = false;
    }  // if

    nextRead = millis() + 50;
  }
}  // loop()

#endif

// End
