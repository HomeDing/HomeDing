/**
 * @file MyElement02.h
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

class MyElement02 : public Element {

public:
  // constants won't change. Used here to set a pin number:
  const int ledPin = LED_BUILTIN;  // the number of the LED pin

  // Variables will change:
  int ledState = LOW;  // ledState used to set the LED

  // Generally, you should use "unsigned long" for variables that hold time
  // The value will quickly become too large for an int to store
  unsigned long previousMillis = 0;  // will store last time LED was updated

  // constants won't change:
  const unsigned long interval = 1000;  // interval at which to blink (milliseconds)

  /** declare the setup the functionality. */
  void setup();

  /** declare run the functionality. */
  void loop();
};

// End
