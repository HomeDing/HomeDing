/**
 * @file DisplayESP32PanelElement.cpp
 *
 * @brief Display Element for TFT displays using the ESP32-S3 specific
 * 16-bit Panel Data interface.
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
 * Changelog:see DisplayESP32PanelElement.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include "displays/DisplayESP32PanelElement.h"

#include "displays/DisplayESP32PanelAdapter.h"
#include "DisplayESP32PanelElement.h"

#define TRACE(...)  // LOGGER_EINFO(__VA_ARGS__)

/* ===== Static factory function ===== */

/// @brief static factory function to create a new DisplayESP32PanelElement
/// @return DisplayESP32PanelElement* created element
Element *DisplayESP32PanelElement::create() {
  return (new DisplayESP32PanelElement());
}  // create()


/* ===== Element functions ===== */

/// @brief Activate the DisplayESP32PanelElement and register a Display Adapter to LCD
void DisplayESP32PanelElement::start() {
  TRACE("start()");

  DisplayAdapter *d = new DisplayESP32PanelAdapter();

  if (d->setup(_board, &config)) {
    bool success = d->start();
    if (success) {
      _board->display = d;
      DisplayElement::start();

    } else {
      LOGGER_EERR("no display found");
      delete d;
    }  // if
  }    // if
}  // start()

// End
