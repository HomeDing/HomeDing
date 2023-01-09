/**
 * @file MyElement03.h
 * @brief minimal Element implementation without any configuration and lifecycle integrations
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
 * 30.12.2021 example implemented
 */

#include <Arduino.h>
#include <HomeDing.h>

#if defined(ESP32) && ! defined(LED_BUILTIN)
// ESP32 DEV boards may do not have a builtin LED. Use Port 25.
#define LED_BUILTIN 25
#endif

class MyElement03 : public Element {

public:
  /** Factory function to create an element. */
  static Element *create();

  /** static variable to ensure registering in static init phase. */
  static bool registered;

  // constants won't change. Used here to set a pin number:
  int ledPin = LED_BUILTIN;  // the number of the LED pin

  // Variables will change:
  int ledState = LOW;  // ledState used to set the LED

  // Generally, you should use "unsigned long" for variables that hold time
  // The value will quickly become too large for an int to store
  unsigned long previousMillis = 0;  // will store last time LED was updated

  // constants won't change:
  unsigned long interval = 1000;  // interval at which to blink (milliseconds)

  /** declare the setup the functionality. */
  void setup() override;

  /** declare the set functionality. */
  bool set(const char *name, const char *value) override;

  /** start / activate the functionality. */
  void start() override;

  /** declare run the functionality. */
  void loop() override;

  /** stop / de-activate the functionality. */
  void term() override;
};

// End
