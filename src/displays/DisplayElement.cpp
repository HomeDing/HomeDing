/**
 * @file DisplayElement.cpp
 * @brief Implementation of the base function for a DisplayXXXElement.
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
 * Changelog:see DisplayElement.h
 */

#include <Arduino.h>
#include <Board.h>
#include <HomeDing.h>

#include "DisplayElement.h"

#define TRACE(...) LOGGER_ETRACE(__VA_ARGS__)

/* ===== Private functions ===== */

void DisplayElement::_reset() {
  // reset
  // if (_resetpin >= 0) {
  //   pinMode(_resetpin, OUTPUT);
  //   digitalWrite(_resetpin, LOW);  // turn low to reset OLED
  //   delay(250);
  //   digitalWrite(_resetpin, HIGH);  // while OLED is running, must set high
  //   delay(250);
  // }  // if
}  // _reset()


void DisplayElement::_newPage(int page) {
  TRACE("newPage %d", page);
  DisplayAdapter *da = _board->display;
  if (da) {
    int oldPage = da->page;

    da->page = constrain(page, 0, da->maxpage);
    // _reset();
    da->start();

    // redraw all display elements
    _board->forEach("display", [this](Element *e) {
      e->set("redraw", "1");
    });
    if (da->page != oldPage) {
      _board->dispatch(_onPage, da->page);
    }
  }
}  // _newPage()


/* ===== Element functions ===== */

/**
 * @brief Constructor of a new DisplayElement.
 */
DisplayElement::DisplayElement() {
  startupMode = Element_StartupMode::System;
}

void DisplayElement::init(Board *board) {
  Element::init(board);

  // use system wide I2C by default
  config.i2cSDA = board->I2cSda;
  config.i2cSCL = board->I2cScl;
    
  // use system wide SPI by default
  // config.spiMISO = ;
  // config.spiMOSI = ;
}

/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool DisplayElement::set(const char *name, const char *value) {
  bool ret = true;
  TRACE("set %s=%s", name, value);
  DisplayAdapter *da = _board->display;

  if (_stricmp(name, PROP_BRIGHTNESS) == 0) {
    int b = _atoi(value);
    config.brightness = constrain(b, 0, 100);
    if (active && da) {
      da->setBrightness(config.brightness);
    }

  } else if (da) {
    // these actions only work with existing display adapter

    if (_stricmp(name, "page") == 0) {
      _newPage(*value ? _atoi(value) : da->page);

    } else if (_stricmp(name, "addpage") == 0) {
      _newPage(da->page + _atoi(value));

    } else if (_stricmp(name, "clear") == 0) {
      da->start();
    }

  } else if (_stricmp(name, "onpage") == 0) {
    // action with current visible page
    _onPage = value;

    // === These properties can only be used during configuration:

  } else if (_stricmp(name, "address") == 0) {
    config.i2cAddress = _atoi(value);

  } else if (_stricmp(name, "resetpin") == 0) {
    config.resetPin = _atopin(value);

  } else if (_stricmp(name, "lightpin") == 0) {
    config.lightPin = _atopin(value);

  } else if (_stricmp(name, "width") == 0) {
    config.width = _atoi(value);

  } else if (_stricmp(name, "height") == 0) {
    config.height = _atoi(value);

  } else if (_stricmp(name, "rotation") == 0) {
    int r = _atoi(value);
    r = (r / 90);
    r = constrain(r, 0, 3);
    config.rotation = r * 90;

  } else {
    ret = Element::set(name, value);
  }  // if

  return (ret);
}  // set()


/**
 * @brief Activate the DisplayElement when a resetpin is given.
 */
void DisplayElement::start() {
  // TRACE("start()");
  Element::start();
  // _reset();
}  // start()

/**
 * @brief push the current value of all properties to the callback.
 */
void DisplayElement::pushState(
  std::function<void(const char *pName, const char *eValue)> callback) {
  Element::pushState(callback);
  DisplayAdapter *da = _board->display;
  if (da) {
    callback(PROP_BRIGHTNESS, String(config.brightness).c_str());
    callback("page", String(_board->display->page).c_str());
  }
}  // pushState()

// End
