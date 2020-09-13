/**
 * @file DisplaySH1106Element.cpp
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
 * Changelog:see DisplaySH1106Element.h
 */

#include <Arduino.h>
#include <Board.h>
#include <HomeDing.h>

#include "DisplaySH1106Element.h"

#include <displays/DisplayAdapterSH1106.h>

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new DisplaySH1106Element
 * @return DisplaySH1106Element* created element
 */
Element *DisplaySH1106Element::create()
{
  return (new DisplaySH1106Element());
} // create()


/* ===== Element functions ===== */

/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool DisplaySH1106Element::set(const char *name, const char *value)
{
  bool ret = DisplayElement::set(name, value);
  return (ret);
} // set()


/**
 * @brief Activate the DisplaySH1106Element and register a Display Adapter to
 * LCD in the board.
 */
void DisplaySH1106Element::start()
{
  DisplayElement::start();

  // TRACE("start()");
  DisplayAdapter *d = (DisplayAdapter *)(new DisplayAdapterSH1106(_address, _height));

  if (d->init(_board)) {
    _board->display = d;

  } else {
    LOGGER_EERR("no display found.");
    delete d;
    active = false;
  } // if
} // start()

// End
