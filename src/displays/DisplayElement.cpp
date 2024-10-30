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
  startupMode = Element_StartupMode::System;
  category = CATEGORY::Display;
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

  if (_stricmp(name, "brightness") == 0) {
    int b = _atoi(value);
    displayConfig.brightness = constrain(b, 0, 100);
    if (active && da) {
      da->setBrightness(displayConfig.brightness);
    }

  } else if (da) {
    // these actions only work with existing display adapter

    if (_stricmp(name, "page") == 0) {
      // switch the page
      _newPage(*value ? _atoi(value) : da->page);

    } else if (_stricmp(name, "addpage") == 0) {
      _newPage(da->page + _atoi(value));

    } else if (name == HomeDing::Actions::Clear) {
      da->start();
    }

  } else if (_stricmp(name, "page") == 0) {
    // da is not (yet) existing

  } else if (_stricmp(name, "onpage") == 0) {
    // action with current visible page
    _onPage = value;

    // === These properties can only be used during configuration:

  } else if (_stricmp(name, "color") == 0) {
    displayConfig.drawColor = _atoColor(value);

  } else if (_stricmp(name, "background") == 0) {
    displayConfig.backgroundColor = _atoColor(value);

  } else if (name == HomeDing::Actions::Border) {
    displayConfig.borderColor = _atoColor(value);

  } else if ((_stricmp(name, "busmode") == 0) || (_stricmp(name, "bus") == 0)) {
    displayConfig.busmode = ListUtils::indexOf(BUSMODE_LIST, value);

  } else if (_stricmp(name, "busspeed") == 0) {
    displayConfig.busSpeed = _atoi(value);


    // ===== parallel busses configuration

  } else if (_stricmp(name, "cspin") == 0) {
    displayConfig.csPin = _atopin(value);

  } else if (_stricmp(name, "dcpin") == 0) {
    displayConfig.dcPin = _atopin(value);

  } else if (_stricmp(name, "wrpin") == 0) {
    displayConfig.wrPin = _atopin(value);

  } else if (_stricmp(name, "rdpin") == 0) {
    displayConfig.rdPin = _atopin(value);

  } else if (_stricmp(name, "buspins") == 0) {
    displayConfig.busPins = value;
    displayConfig.busPins.replace(" ", "");


    // ===== i2c bus parameter

  } else if (name == HomeDing::Actions::Address) {
    displayConfig.i2cAddress = _atoi(value);

    // ===== spi bus parameter

  } else if (_stricmp(name, "spimosi") == 0) {
    displayConfig.spiMOSI = _atopin(value);

  } else if (_stricmp(name, "spimiso") == 0) {
    displayConfig.spiMISO = _atopin(value);

  } else if (_stricmp(name, "spiclk") == 0) {
    displayConfig.spiCLK = _atopin(value);

  } else if (_stricmp(name, "spics") == 0) {
    displayConfig.csPin = _atopin(value);  // please use csPin, deprecated

  } else if (_stricmp(name, "spidc") == 0) {
    displayConfig.dcPin = _atopin(value);  // please use dcPin, deprecated


  } else if (name == HomeDing::Actions::Invert) {
    displayConfig.invert = _atob(value);

  } else if (_stricmp(name, "ips") == 0) {
    displayConfig.ips = _atob(value);

  } else if (_stricmp(name, "resetpin") == 0) {
    displayConfig.resetPin = _atopin(value);

  } else if (_stricmp(name, "lightpin") == 0) {
    displayConfig.lightPin = _atopin(value);

    // ===== Display settings

  } else if (name == HomeDing::Actions::Width) {
    displayConfig.width = _atoi(value);

  } else if (name == HomeDing::Actions::Height) {
    displayConfig.height = _atoi(value);

  } else if (_stricmp(name, "rotation") == 0) {
    int r = _atoi(value);
    r = (r / 90);
    r = constrain(r, 0, 3);
    displayConfig.rotation = r * 90;

  } else if (_stricmp(name, "rowOffset") == 0) {
    displayConfig.rowOffset = _atoi(value);

  } else if (_stricmp(name, "colOffset") == 0) {
    displayConfig.colOffset = _atoi(value);

  } else {
    ret = Element::set(name, value);
  }  // if

  return (ret);
}  // set()


/**
 * @brief Activate the Element.
 */
void DisplayElement::start() {
  LOGGER_EERR("start() called without display Adapter parameter");

  // DisplayAdapter *da = _board->display;
  // if (da) {
  //   Element::start();
  //   da->setBrightness(displayConfig.brightness);
  //   da->setBackgroundColor(displayConfig.backgroundColor);

  // } else {
  //   LOGGER_EERR("start failed.");
  // }
}  // start()


/// @brief Activate the Element using the given adapter.
void DisplayElement::start(DisplayAdapter *displayAdapter) {
  TRACE("start()");
  if (displayAdapter) {
    resetPin;
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
    callback("brightness", String(displayConfig.brightness).c_str());
    callback("page", String(_board->display->page).c_str());
  }
}  // pushState()

// End
