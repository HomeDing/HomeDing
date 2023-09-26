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
#include <HomeDing.h>

#include "DisplayElement.h"

#define TRACE(...)  // LOGGER_ETRACE(__VA_ARGS__)

// ===== private functions =====

void DisplayElement::_newPage(int page) {
  TRACE("newPage %d", page);
  DisplayAdapter *da = _board->display;
  if (da) {
    int oldPage = da->page;

    da->page = constrain(page, 0, da->maxpage);
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


// ===== Element functions =====

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
}

/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool DisplayElement::set(const char *name, const char *value) {
  bool ret = true;
  TRACE("set %s=%s", name, value);
  DisplayAdapter *da = _board->display;

  if (_stricmp(name, "brightness") == 0) {
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

  } else if (_stricmp(name, "color") == 0) {
    config.drawColor = _atoColor(value);

  } else if (_stricmp(name, "background") == 0) {
    config.backgroundColor = _atoColor(value);

  } else if (_stricmp(name, "border") == 0) {
    config.borderColor = _atoColor(value);

  } else if (_stricmp(name, "busmode") == 0) {
    if (_stricmp(value, "spi") == 0) {
      config.busmode = BUSMODE_SPI;
    } else if (_stricmp(value, "i2c") == 0) {
      config.busmode = BUSMODE_I2C;
    }

    // ===== i2c bus parameter

  } else if (_stricmp(name, "address") == 0) {
    config.i2cAddress = _atoi(value);

    // ===== spi bus parameter

  } else if (_stricmp(name, "spimosi") == 0) {
    config.spiMOSI = _atopin(value);

  } else if (_stricmp(name, "spimiso") == 0) {
    config.spiMISO = _atopin(value);

  } else if (_stricmp(name, "spiclk") == 0) {
    config.spiCLK = _atopin(value);

  } else if (_stricmp(name, "spics") == 0) {
    config.spiCS = _atopin(value);

  } else if (_stricmp(name, "spidc") == 0) {
    config.spiDC = _atopin(value);


  } else if (_stricmp(name, "invert") == 0) {
    config.invert = _atob(value);

  } else if (_stricmp(name, "ips") == 0) {
    config.ips = _atob(value);

  } else if (_stricmp(name, "resetpin") == 0) {
    config.resetPin = _atopin(value);

  } else if (_stricmp(name, "lightpin") == 0) {
    config.lightPin = _atopin(value);

    // ===== Display settings

  } else if (_stricmp(name, "width") == 0) {
    config.width = _atoi(value);

  } else if (_stricmp(name, "height") == 0) {
    config.height = _atoi(value);

  } else if (_stricmp(name, "rotation") == 0) {
    int r = _atoi(value);
    r = (r / 90);
    r = constrain(r, 0, 3);
    config.rotation = r * 90;

  } else if (_stricmp(name, "rowOffset") == 0) {
    config.rowOffset = _atoi(value);

  } else if (_stricmp(name, "colOffset") == 0) {
    config.colOffset = _atoi(value);

  } else {
    ret = Element::set(name, value);
  }  // if

  return (ret);
}  // set()


/**
 * @brief Activate the Element.
 */
void DisplayElement::start() {
  Element::start();
  DisplayAdapter *da = _board->display;

  if (active && da) {
    da->setBrightness(config.brightness);
    da->setBackgroundColor(config.backgroundColor);
  }
}  // start()


/**
 * @brief push the current value of all properties to the callback.
 */
void DisplayElement::pushState(
  std::function<void(const char *pName, const char *eValue)> callback) {
  Element::pushState(callback);
  DisplayAdapter *da = _board->display;
  if (da) {
    callback("brightness", String(config.brightness).c_str());
    callback("page", String(_board->display->page).c_str());
  }
}  // pushState()

// End
