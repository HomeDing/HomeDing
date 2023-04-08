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

#define TRACE(...) // LOGGER_ETRACE(__VA_ARGS__)


#define DATA_PIN _pins[0]
#define CLOCK_PIN _pins[1]


/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new APA102Element
 * @return APA102Element* created element
 */
Element *APA102Element::create() {
  return (new APA102Element());
}  // create()


/* ===== Strip specific function ===== */

void APA102Element::_sendByte(uint8_t b) {
  uint8_t dataPin = DATA_PIN;
  uint8_t clockPin = CLOCK_PIN;

  // Serial.printf("%02x ", b);

  digitalWrite(dataPin, (b & 0b10000000) > 0);
  digitalWrite(clockPin, HIGH);
  digitalWrite(clockPin, LOW);
  digitalWrite(dataPin, (b & 0b01000000) > 0);
  digitalWrite(clockPin, HIGH);
  digitalWrite(clockPin, LOW);
  digitalWrite(dataPin, (b & 0b00100000) > 0);
  digitalWrite(clockPin, HIGH);
  digitalWrite(clockPin, LOW);
  digitalWrite(dataPin, (b & 0b00010000) > 0);
  digitalWrite(clockPin, HIGH);
  digitalWrite(clockPin, LOW);
  digitalWrite(dataPin, (b & 0b00001000) > 0);
  digitalWrite(clockPin, HIGH);
  digitalWrite(clockPin, LOW);
  digitalWrite(dataPin, (b & 0b00000100) > 0);
  digitalWrite(clockPin, HIGH);
  digitalWrite(clockPin, LOW);
  digitalWrite(dataPin, (b & 0b00000010) > 0);
  digitalWrite(clockPin, HIGH);
  digitalWrite(clockPin, LOW);
  digitalWrite(dataPin, (b & 0b00000001) > 0);
  digitalWrite(clockPin, HIGH);
  digitalWrite(clockPin, LOW);
}


void APA102Element::show() {
  TRACE("apa102:show(%d) %d", _count, _brightness);
  StripeElement::show();

  // brightness byte 0xE0 + 0..31
  int b = 0xE0 | (_brightness * 31 / 100);

  _sendByte(0);
  _sendByte(0);
  _sendByte(0);
  _sendByte(0);

  for (int i = 0; i < _count; i++) {
    uint32_t col = pixels[i];
    _sendByte(b);
    _sendByte(col & 0x00FF);
    _sendByte((col >> 8) & 0x00FF);
    _sendByte((col >> 16) & 0x00FF);
  }

  // _sendByte(0); _sendByte(0); _sendByte(0); _sendByte(0);
  _sendByte(0xFF);
  _sendByte(0xFF);
  _sendByte(0xFF);
  _sendByte(0xFF);

  // Serial.println();

}  // show()


/* ===== Element functions ===== */

/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool APA102Element::set(const char *name, const char *value) {
  TRACE("apa102::set %s=%s", name, value);
  bool ret = true;

  if (StripeElement::set(name, value)) {
    // all done
  } else if (_stricmp(name, "datapin") == 0) {
    DATA_PIN = _atopin(value);

  } else if (_stricmp(name, "clockpin") == 0) {
    CLOCK_PIN = _atopin(value);

  } else {
    ret = false;
  }  // if

  return (ret);
}  // set()


/**
 * @brief Activate the APA102Element.
 */
void APA102Element::start() {
  StripeElement::start();
  uint8_t dataPin = DATA_PIN;
  uint8_t clockPin = CLOCK_PIN;

  pinMode(dataPin, OUTPUT);
  digitalWrite(dataPin, LOW);
  pinMode(clockPin, OUTPUT);
  digitalWrite(clockPin, LOW);
}  // start()


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void APA102Element::loop() {
  StripeElement::loop();
}  // loop()


// End
