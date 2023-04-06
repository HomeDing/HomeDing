/**
 * @file APA102Element.cpp
 * @brief Output Element to control neopixel / WS2812 based LED stripes on a GPIO pin.
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
 * Changelog:see APA102Element.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <light/APA102Element.h>

#define TRACE(...) LOGGER_ETRACE(__VA_ARGS__)

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new APA102Element
 * @return APA102Element* created element
 */
Element *APA102Element::create() {
  return (new APA102Element());
}  // create()


/* ===== Strip specific function ===== */

void APA102Element::show() {
  TRACE("show(%d)", _count);
  StripeElement::show();
}  // show()


/* ===== Element functions ===== */

/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool APA102Element::set(const char *name, const char *pValue) {
  TRACE("set %s=%s", name, pValue);
  bool ret = StripeElement::set(name, pValue);

  return (ret);
}  // set()


/**
 * @brief Activate the APA102Element.
 */
void APA102Element::start() {
  StripeElement::start();
}  // start()


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void APA102Element::loop() {
  StripeElement::loop();
}  // loop()


// End
