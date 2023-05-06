/**
 * @file MY9291Element.cpp
 * @brief Element for using the Y9291 LED driver chip.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD 3-Clause style license,
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:see MY9291Element.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#if defined(ESP8266)

#include <light/MY9291Element.h>

#include "my92xx.h"

my92xx *_my92xx;

#define TRACE(...)  // LOGGER_ETRACE(__VA_ARGS__)

/* ===== Define local constants and often used strings ===== */

#define DI_PIN _pins[0]
#define DCKI_PIN _pins[1]


/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new MY9291Element
 * @return MY9291Element* created element
 */
Element *MY9291Element::create() {
  return (new MY9291Element());
}  // create()


/* ===== Element functions ===== */


void MY9291Element::init(Board *board) {
  LightElement::init(board);
  DI_PIN = 4;
  DCKI_PIN = 5;
}  // init()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool MY9291Element::set(const char *name, const char *value) {
  LOGGER_INFO("set %s=%s", name, value);

  bool ret = LightElement::set(name, value);

  if (ret) {

  } else if (_stricmp(name, "datapin") == 0) {
    DI_PIN = _atopin(value);

  } else if (_stricmp(name, "clockpin") == 0) {
    DCKI_PIN = _atopin(value);
  }  // if

  return (ret);
}  // set()


/**
 * @brief Activate the MY9291Element.
 */
void MY9291Element::start() {
  // Verify parameters
  if ((DI_PIN >= 0) && (DCKI_PIN >= 0)) {
    if (!_my92xx) {
      _my92xx = new my92xx(MY92XX_MODEL_MY9291, 1, DI_PIN, DCKI_PIN, MY92XX_COMMAND_DEFAULT);
    }
    LightElement::start();
  }  // if

}  // start()


/**
 * @brief Give some processing time for update of the output.
 */
void MY9291Element::loop() {
  if (needUpdate) {
    uint32_t color = (enabled ? _outColor : 0);

    int white = (color & 0xFF000000) >> 24;
    int red = (color & 0x00FF0000) >> 16;
    int green = (color & 0x0000FF00) >> 8;
    int blue = (color & 0x000000FF);

    _my92xx->setState(true);
    _my92xx->setChannel(0, red * _brightness / 100);
    _my92xx->setChannel(1, green * _brightness / 100);
    _my92xx->setChannel(2, blue * _brightness / 100);
    _my92xx->setChannel(3, white);
    _my92xx->update();
  }  // if
  LightElement::loop();
  // needUpdate = false;
}  // loop()

#endif
