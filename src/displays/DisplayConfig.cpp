/**
 * @file HomeDing::DisplayConfig.cpp
 * @brief Implementation of the display base function for a DisplayXXXElement.
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
 * Changelog:see DisplayConfig.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include "DisplayConfig.h"

namespace HomeDing {

HomeDing::DisplayConfig displayConfig;

DisplayConfig::DisplayConfig() {
  // initialization values.
  busmode = BUSMODE_ANY;
  brightness = 50;

  /** Default Draw & Background Color */
  drawColor = RGB_WHITE;
  backgroundColor = RGB_BLACK;
  borderColor = RGB_WHITE;

  resetPin = -1;
  lightPin = -1;

  busSpeed = -1;
  csPin = -1;
  dcPin = -1;
  wrPin = -1;
  rdPin = -1;
}

} // namespace