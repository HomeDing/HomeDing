/**
 * @file P9813Element.cpp
 * @brief The P8913Element implements communication with P9813 LED driver chips to implement RGB lights.
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
 * Changelog:see P9813Element.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include "P9813Element.h"


#define TRACE(...) // LOGGER_ETRACE(__VA_ARGS__)

// from: https://github.com/pjpmarques/ChainableLED/blob/master/ChainableLED.cpp

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new P9813Element
 * @return P9813Element* created element
 */
Element *P9813Element::create()
{
  return (new P9813Element());
} // create()


/* ===== Element functions ===== */

/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool P9813Element::set(const char *name, const char *value)
{
  // TRACE("set %s=%s", name, value);
  bool ret = LightElement::set(name, value);

  if (!ret) {
    if (_stricmp(name, "datapin") == 0) {
      // "_pins[0]" is used for data
      _pins[0] = _atopin(value);

    } else if (_stricmp(name, "clockpin") == 0) {
      // "_pins[1]" is used for clock
      _pins[1] = _atopin(value);
    }
  } // if

  return (ret);
} // set()


// send a 32-bit data set for P9813, can be used for start, stop and data.
void P9813Element::sendPixelData(const uint32_t data)
{
  int datapin = _pins[0];
  int clkpin = _pins[1];
  uint32_t bits = data;

  // Send starting with the MSB
  for (int i = 0; i < 32; i++) {
    // set data signal
    // digitalWrite(datapin, ((b & 0x80) != 0));
    if ((bits & 0x80000000) != 0)
      digitalWrite(datapin, HIGH);
    else
      digitalWrite(datapin, LOW);

    // send clock signal
    digitalWrite(clkpin, LOW);
    delayMicroseconds(20);
    digitalWrite(clkpin, HIGH);
    delayMicroseconds(20);
    bits <<= 1;
  } // for
} // sendPixelData()


void P9813Element::setOutput(String value)
{
  uint32_t col = _atoColor(value.c_str());

  // Start frame
  sendPixelData(0x00000000);

  uint8_t blue =  (((col & 0x000000FF)) * brightness) / 100;
  uint8_t green = (((col & 0x0000FF00) >> 8) * brightness) / 100;
  uint8_t red =   (((col & 0x00FF0000) >> 16) * brightness) / 100;

  // send prefix
  uint32_t data = 0xC0000000; // first 2 bits (flag bits) set
  if (! (blue & 0x80) ) data |= 0x20000000;
  if (! (blue & 0x40) ) data |= 0x10000000;
  if (! (green & 0x80)) data |= 0x08000000;
  if (! (green & 0x40)) data |= 0x04000000;
  if (! (red & 0x80)  ) data |= 0x02000000;
  if (! (red & 0x40)  ) data |= 0x01000000;

  // colors incl. all bits
  data |= (blue << 16);
  data |= (green << 8);
  data |= (red);
  sendPixelData(data);

  // Terminate frame
  sendPixelData(0x00000000);
} // setOutput()


/**
 * @brief Activate the P9813Element.
 */
void P9813Element::start()
{
  TRACE("start()");

  // Verify parameters
  if ((_pins[0] >= 0) && (_pins[1] >= 0)) {
    pinMode(_pins[0], OUTPUT);
    pinMode(_pins[1], OUTPUT);
    LightElement::start();
  } // if

} // start()


/**
 * @brief push the current value of all properties to the callback.
 */
void P9813Element::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  LightElement::pushState(callback);
} // pushState()


/* ===== Register the Element ===== */

// As long as the Element is project specific or is a element always used
// the registration is placed here without using a register #define.

// When transferred to the HomeDing library a #define like the
// HOMEDING_INCLUDE_XXX should be used to allow the sketch to select the
// available Elements. See <HomeDing.h> the move these lines to P9813Element.h:

// #ifdef HOMEDING_REGISTER
// Register the P9813Element onto the ElementRegistry.
bool P9813Element::registered =
    ElementRegistry::registerElement("p9813", P9813Element::create);
// #endif

// End
