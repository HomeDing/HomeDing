/**
 * @file DisplayAdapter.cpp
 *
 * @brief Implementing base functionality for Display Adapters.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license.
 * See https://www.mathertel.de/License.aspx.
 *
 * Changelog:see DisplayElement.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include "DisplayAdapter.h"

#define TRACE(...)  // LOGGER_TRACE(__VA_ARGS__)

DisplayAdapter::DisplayAdapter() {
  color = 0x00FFFFFF;      // white
  backColor = 0x00000000;  // black
}


/// @brief setup a fresh Display Adapter
/// @param b Board Reference
/// @param c DisplayConfig Data
/// @return true
bool DisplayAdapter::setup(Board *b, struct DisplayConfig *c) {
  board = b;
  conf = c;

  displayBox.x_min = 0;
  displayBox.y_min = 0;
  if ((conf->rotation / 90) % 4 == 0) {
    displayBox.x_max = c->width;
    displayBox.y_max = c->height;
  } else {
    displayBox.x_max = c->height;
    displayBox.y_max = c->width;
  }
  return (true);
}  // setup()


/// @brief Start the display.
/// @return true when the display is ready for operation. Otherwise false.
bool DisplayAdapter::start() {
  if (conf->lightPin) {
#if defined(ESP8266)
    pinMode(conf->lightPin, OUTPUT);
    analogWriteRange(100);

#elif (defined(ESP32))
    _lightChannel = board->nextLedChannel++;
    TRACE("display chan %d %d", _lightChannel, conf->lightPin);
    ledcSetup(_lightChannel, 8000, 10);
    ledcAttachPin(conf->lightPin, _lightChannel);
#endif
    setBrightness(conf->brightness);
  }

  return (true);
};


/// @brief set brightness for panel lightning.
/// @param bright new brightness in range 0...100
void DisplayAdapter::setBrightness(uint8_t bright) {
  TRACE("setBrightness %d %d", conf->lightPin, bright);
  if (conf->lightPin) {
#if defined(ESP8266)
    analogWrite(conf->lightPin, bright);
#elif (defined(ESP32))
    uint32_t duty = (bright * 1024) / 100;
    TRACE("display duty %d %d", _lightChannel, duty);
    ledcWrite(_lightChannel, duty);
#endif
  }
};
