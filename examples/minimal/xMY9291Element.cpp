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
#include <Board.h>
#include <Element.h>

#include <HomeDing.h>

#include "xMY9291Element.h"

#include "my92xx.h"

my92xx *_my92xx;

/* ===== Define local constants and often used strings ===== */

#define DI_PIN _pins[0]
#define DCKI_PIN _pins[1]


/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new MY9291Element
 * @return MY9291Element* created element
 */
Element *MY9291Element::create()
{
  return (new MY9291Element());
} // create()


/* ===== Element functions ===== */

MY9291Element::MY9291Element()
{
  // adjust startupMode when Network (default) is not applicable.
  // startupMode = Element_StartupMode::System;
}


void MY9291Element::init(Board *board)
{
  TRACE("init()");
  LightElement::init(board);
  DI_PIN = 4;
  DCKI_PIN = 5;
} // init()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool MY9291Element::set(const char *name, const char *value)
{
  LOGGER_INFO("set %s=%s", name, value);

  bool ret = LightElement::set(name, value);

  if (ret) {

  } else if (_stricmp(name, "datapin") == 0) {
    DI_PIN = _atopin(value);

  } else if (_stricmp(name, "clockpin") == 0) {
    DCKI_PIN = _atopin(value);
  } // if

  return (ret);
} // set()


/**
 * @brief Activate the MY9291Element.
 */
void MY9291Element::start()
{
  TRACE("start()");

  // Verify parameters

  if ((DI_PIN >= 0) && (DCKI_PIN >= 0)) {
    Element::start();
    if (!_my92xx) {
      _my92xx = new my92xx(MY92XX_MODEL_MY9291, 1, DI_PIN, DCKI_PIN, MY92XX_COMMAND_DEFAULT);
    }
    setOutput(value);
  } // if

} // start()


void MY9291Element::setOutput(String value)
{
  uint32_t col = _atoColor(value.c_str());

  int white = (col & 0xFF000000) >> 24;
  int red = (col & 0x00FF0000) >> 16;
  int green = (col & 0x0000FF00) >> 8;
  int blue = (col & 0x000000FF);

  _my92xx->setState(true);
  _my92xx->setChannel(0, red * brightness / 100);
  _my92xx->setChannel(1, green * brightness / 100);
  _my92xx->setChannel(2, blue * brightness / 100);
  _my92xx->setChannel(3, white);

  _my92xx->update();
} // setOutput()


/* ===== Register the Element ===== */

// As long as the Element is project specific or is a element always used
// the registration is placed here without using a register #define.

// When transferred to the HomeDing library a #define like the
// HOMEDING_INCLUDE_XXX should be used to allow the sketch to select the
// available Elements. See <HomeDing.h> the move these lines to MY9291Element.h:

// #ifdef HOMEDING_REGISTER
// Register the MY9291Element onto the ElementRegistry.
bool MY9291Element::registered =
    ElementRegistry::registerElement("my9291", MY9291Element::create);
// #endif

// End
