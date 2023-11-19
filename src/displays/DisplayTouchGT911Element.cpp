/**
 * @file DisplayTouchGT911Element.cpp
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:see DisplayTouchGT911Element.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <displays/DisplayButtonElement.h>
#include <displays/DisplayTouchGT911Element.h>

#include <Wire.h>


#define TRACE(...) // LOGGER_ETRACE(__VA_ARGS__)

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new DisplayTouchGT911Element
 * @return DisplayTouchGT911Element* created element
 */
Element *DisplayTouchGT911Element::create() {
  return (new DisplayTouchGT911Element());
}  // create()


/* ===== Private functions ===== */

bool DisplayTouchGT911Element::pullSensorData() {
  GDTpoint_t points[GT911_MAX_CONTACTS];
  uint8_t count = tp->getTouchPoints(points);

  if (count > 0) {
    // as of now only interested in the first.
    lastX = points[0].x;
    lastY = points[0].y;
    return (true);
  } else {
    return (false);
  }
}  // pullSensorData()


/* ===== Element functions ===== */

/**
 * @brief Activate the DisplayTouchGT911Element.
 */
void DisplayTouchGT911Element::start() {
  TRACE("DisplayTouchGT911Element::start()");

  // // set the digital pin as output:
  tp = new GT911(_interruptPin, _resetPin, _width, _height);

  if (tp) {
    tp->init(_address);
    DisplayTouchElement::start();
  }
}  // start()


// End
