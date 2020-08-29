/**
 * @file DisplaySSD1306Element.cpp
 * @brief Element Template class.
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
 * Changelog:see DisplaySSD1306Element.h
 */

#include <Arduino.h>
#include <Board.h>
#include <HomeDing.h>

#include "DisplaySSD1306Element.h"

#include <displays/DisplayAdapterSSD1306.h>

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new DisplaySSD1306Element
 * @return DisplaySSD1306Element* created element
 */
Element *DisplaySSD1306Element::create()
{
  return (new DisplaySSD1306Element());
} // create()


/* ===== Element functions ===== */

/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool DisplaySSD1306Element::set(const char *name, const char *value)
{
  bool ret = true;

  if (_stricmp(name, "enable") == 0) {
    if (_resetpin >= 0) {
      pinMode(_resetpin, OUTPUT);
      digitalWrite(_resetpin, LOW); // turn low to reset OLED
      delay(50);
      digitalWrite(_resetpin, HIGH); // while OLED is running, must set high
    } // if
    DisplayAdapter *d = _board->display;
    d->init(_board);
    d->drawText(0, 0, 10, "enabled...");
    d->flush();

  } else if (_stricmp(name, "enable2") == 0) {
    DisplayAdapter *d = _board->display;
    d->init(_board);
    d->drawText(0, 0, 10, "enabled...");
    d->flush();

  } else {
    ret = DisplayElement::set(name, value);
  } // if

  return (ret);
} // set()


/**
 * @brief Activate the DisplaySSD1306Element and register a Display Adapter to
 * LCD in the board.
 */
void DisplaySSD1306Element::start()
{
  DisplayElement::start();
  LOGGER_ETRACE("start()");

  DisplayAdapter *d = (DisplayAdapter *)(new DisplayAdapterSSD1306(_address, _height));
  if (d->init(_board)) {
    _board->display = d;

  } else {
    LOGGER_EERR("no display found.");
    delete d;
    active = false;
  } // if
} // start()

// End
