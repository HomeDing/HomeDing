/**
 * @file DisplayTouchFT6336Element.cpp
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:see DisplayTouchFT6336Element.h
 */

#include <Arduino.h>
#include <HomeDing.h>
#include <WireUtils.h>

#include <displays/DisplayButtonElement.h>
#include <displays/DisplayTouchFT6336Element.h>

#include <Wire.h>


#define TRACE(...) // LOGGER_ETRACE(__VA_ARGS__)

// device has a fixed i2c address
#define I2C_ADDR 0x38

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new DisplayTouchFT6336Element
 * @return DisplayTouchFT6336Element* created element
 */
Element *DisplayTouchFT6336Element::create() {
  return (new DisplayTouchFT6336Element());
}  // create()


/* ===== Private functions ===== */

bool DisplayTouchFT6336Element::pullSensorData() {
  if (_found) {
    uint8_t registers[15];  // 0..E

    WireUtils::readBuffer(I2C_ADDR, 0, registers, 7);

    int count = registers[2];

    if (count > 0) {
      lastX = ((registers[3] & 0x0F) << 8) + registers[4];
      lastY = ((registers[5] & 0x0F) << 8) + registers[6];
      Serial.printf("%4d %4d\n", lastX, lastY);
      return (true);
    }
  }
  return (false);
} // pullSensorData()


/* ===== Element functions ===== */

/// @brief Start the DisplayTouchFT6336 Element.
void DisplayTouchFT6336Element::start() {
  TRACE("start()");

  // find chip on the i2c bus
  if (WireUtils::exists(I2C_ADDR)) {
    TRACE("found!");
    DisplayTouchElement::start();
    _found = true;
  }
}  // start()


// End
