/**
 * @file DisplayTouchCST816Element.cpp
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:see DisplayTouchCST816Element.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <displays/DisplayButtonElement.h>
#include <displays/DisplayTouchCST816Element.h>

#include <Wire.h>

#define TRACE(...) LOGGER_ETRACE(__VA_ARGS__)

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new DisplayTouchCST816Element
 * @return DisplayTouchCST816Element* created element
 */
Element *DisplayTouchCST816Element::create() {
  return (new DisplayTouchCST816Element());
}  // create()


/* ===== Private functions ===== */

bool DisplayTouchCST816Element::pullSensorData() {
  // TRACE("pullSensorData()");

  CST816point_t points;

  if (otp->getTouchPoints(&points)) {
    lastX = points.x;
    lastY = points.y;
    return (true);
  }

  return (false);
}  // pullSensorData()


/* ===== Element functions ===== */

/**
 * @brief Activate the DisplayTouchCST816Element.
 */
void DisplayTouchCST816Element::start() {
  TRACE("CST816::start()");

  // _interruptPin, _resetPin, _width, _height

  pinMode(15, OUTPUT);  // TFT poweron
  digitalWrite(15, HIGH);

  otp = new CST816(_interruptPin, _resetPin, _width, _height);

  if (otp) {
    otp->begin(Wire);
    DisplayTouchElement::start();
  }
}  // start()


// End
