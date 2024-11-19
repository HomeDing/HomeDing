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

using namespace HomeDing;

DisplayElement::DisplayElement() {
  startupMode = Element::STARTUPMODE::System;
  category = CATEGORY::Display; // no looping
}

// ===== private functions =====

void DisplayElement::_newPage(int page) {
  TRACE("newPage %d", page);
  DisplayAdapter *da = _board->display;
  if (da) {
    int oldPage = da->page;

    da->page = constrain(page, 0, da->maxpage);

    da->clear();
    // redraw all DisplayOutput elements
    _board->forEach(CATEGORY::Widget, [this](Element *e) {
      TRACE("do %s", e->id);
      e->set(HomeDing::Actions::Redraw, "1");
    });
    if (da->page != oldPage) {
      HomeDing::Actions::push(_onPage, String(da->page).c_str());
    }
  }
}  // _newPage()


// ===== Element functions =====


void DisplayElement::init(Board *board) {
  Element::init(board);

  // use system wide I2C by default
  displayConfig.i2cSDA = board->I2cSda;
  displayConfig.i2cSCL = board->I2cScl;

  // use system wide SPI by default
  displayConfig.spiCLK = board->spiCLK;
  displayConfig.spiMOSI = board->spiMOSI;
  displayConfig.spiMISO = board->spiMISO;
}  // init()


///  @brief Set a parameter or property to a new value or start an action.
bool DisplayElement::set(const char *name, const char *value) {
  bool ret = true;
  TRACE("set %s=%s", name, value);
  DisplayAdapter *da = _board->display;
  int value_int = _atoi(value);
  int value_pin = _atopin(value);

  if (name == HomeDing::Actions::Brightness) {
    int b = value_int;
    displayConfig.brightness = constrain(b, 0, 100);
    if (active && da) {
      da->setBrightness(displayConfig.brightness);
    }

  } else if (da) {
    // these actions only work with existing display adapter

    if (_stricmp(name, "page") == 0) {
      // switch the page
      _newPage(*value ? value_int : da->page);

    } else if (_stricmp(name, "addpage") == 0) {
      _newPage(da->page + value_int);

    } else if (name == HomeDing::Actions::Clear) {
      da->start();
    }

  } else if (_stricmp(name, "page") == 0) {
    // da is not (yet) existing

  } else if (_stricmp(name, "onpage") == 0) {
    // action with current visible page
    _onPage = value;

    // === These properties can only be used during configuration:

  } else if (_stricmp(name, "background") == 0) {
    displayConfig.backgroundColor = _atoColor(value);

  } else if ((_stricmp(name, "busmode") == 0) || (_stricmp(name, "bus") == 0)) {
    displayConfig.busmode = ListUtils::indexOf(BUSMODE_LIST, value);

  } else if (_stricmp(name, "busspeed") == 0) {
    displayConfig.busSpeed = value_int;


    // ===== bus configurations for any bus

  } else if (_stricmp(name, "cspin") == 0) {
    displayConfig.csPin = value_pin;

  } else if (_stricmp(name, "dcpin") == 0) {
    displayConfig.dcPin = value_pin;

  } else if (_stricmp(name, "wrpin") == 0) {
    displayConfig.wrPin = value_pin;

  } else if (_stricmp(name, "rdpin") == 0) {
    displayConfig.rdPin = value_pin;


    // ===== parallel busses configuration

  } else if (_stricmp(name, "buspins") == 0) {
    displayConfig.busPins = value;
    displayConfig.busPins.replace(" ", "");

  } else if (_stricmp(name, "depin") == 0) {
    displayConfig.dePin = value_pin;


  } else if (_stricmp(name, "hsyncpin") == 0) {
    displayConfig.hsync_pin = value_pin;

  } else if (_stricmp(name, "hsyncpolarity") == 0) {
    displayConfig.hsync_polarity = value_int;

  } else if (_stricmp(name, "hsyncpulsewidth") == 0) {
    displayConfig.hsync_pulse_width = value_int;

  } else if (_stricmp(name, "hsyncfrontporch") == 0) {
    displayConfig.hsync_front_porch = value_int;

  } else if (_stricmp(name, "hsyncbackporch") == 0) {
    displayConfig.hsync_back_porch = value_int;


  } else if (_stricmp(name, "vsyncpin") == 0) {
    displayConfig.vsync_pin = value_pin;

  } else if (_stricmp(name, "vsyncpolarity") == 0) {
    displayConfig.vsync_polarity = value_int;

  } else if (_stricmp(name, "vsyncpulsewidth") == 0) {
    displayConfig.vsync_pulse_width = value_int;

  } else if (_stricmp(name, "vsyncfrontporch") == 0) {
    displayConfig.vsync_front_porch = value_int;

  } else if (_stricmp(name, "vsyncbackporch") == 0) {
    displayConfig.vsync_back_porch = value_int;

  } else if (_stricmp(name, "pclkpin") == 0) {
    displayConfig.pclk_pin = value_pin;

    // ===== i2c bus parameter

  } else if (name == HomeDing::Actions::Address) {
    displayConfig.i2cAddress = value_int;

    // ===== spi bus parameter

  } else if (_stricmp(name, "spimosi") == 0) {
    displayConfig.spiMOSI = value_pin;

  } else if (_stricmp(name, "spimiso") == 0) {
    displayConfig.spiMISO = value_pin;

  } else if (_stricmp(name, "spiclk") == 0) {
    displayConfig.spiCLK = value_pin;

  } else if (_stricmp(name, "spics") == 0) {
    displayConfig.csPin = value_pin;  // please use csPin, deprecated

  } else if (_stricmp(name, "spidc") == 0) {
    displayConfig.dcPin = value_pin;  // please use dcPin, deprecated


  } else if (name == HomeDing::Actions::Invert) {
    displayConfig.invert = _atob(value);

  } else if (_stricmp(name, "ips") == 0) {
    displayConfig.ips = _atob(value);

  } else if (_stricmp(name, "resetpin") == 0) {
    displayConfig.resetPin = value_pin;

  } else if (_stricmp(name, "lightpin") == 0) {
    displayConfig.lightPin = value_pin;

    // ===== Display settings

  } else if (name == HomeDing::Actions::Width) {
    displayConfig.width = value_int;

  } else if (name == HomeDing::Actions::Height) {
    displayConfig.height = value_int;

  } else if (_stricmp(name, "rotation") == 0) {
    int r = value_int;
    r = (r / 90);
    r = constrain(r, 0, 3);
    displayConfig.rotation = r * 90;

  } else if (_stricmp(name, "rowOffset") == 0) {
    displayConfig.rowOffset = value_int;

  } else if (_stricmp(name, "colOffset") == 0) {
    displayConfig.colOffset = value_int;

  } else {
    ret = Element::set(name, value);
  }  // if

  return (ret);
}  // set()


/// @brief Activate the Element using the given adapter.
void DisplayElement::start(DisplayAdapter *displayAdapter) {
  TRACE("start()");
  if (displayAdapter) {
    if (displayAdapter->setup(_board)) {
      bool success = displayAdapter->start();
      if (success) {

        _board->display = displayAdapter;
        HomeDing::displayAdapter = displayAdapter;

        displayAdapter->setBrightness(displayConfig.brightness);

        // should be removed in the future:
        displayAdapter->setColor(displayConfig.drawColor);
        displayAdapter->setBackgroundColor(displayConfig.backgroundColor);

      } else {
        delete displayAdapter;
        LOGGER_EERR("start failed.");
      }
    }
  }
  Element::start();
}  // start(da)


/**
 * @brief push the current value of all properties to the callback.
 */
void DisplayElement::pushState(
  std::function<void(const char *pName, const char *eValue)> callback) {
  Element::pushState(callback);
  DisplayAdapter *da = _board->display;
  if (da) {
    callback(HomeDing::Actions::Brightness, _printInteger(displayConfig.brightness));
    callback("page", _printInteger(_board->display->page));
  }
}  // pushState()

// End
