/**
 * @file MyElement01.h
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

#if defined(ESP32)
// ESP32 DEV boards do not have a builtin LED. Use Port 25.
#define LED_BUILTIN 25
#endif

class MyElement01 : public Element {

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

  /** setup the functionality. */
  void setup() {
    // set the digital pin as output:
    pinMode(ledPin, OUTPUT);
  }

  /** run the functionality. */
  void loop() {
    // here is where you'd put code that needs to be running all the time.

    // check to see if it's time to blink the LED; that is, if the difference
    // between the current time and last time you blinked the LED is bigger than
    // the interval at which you want to blink the LED.
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval) {
      // save the last time you blinked the LED
      previousMillis = currentMillis;

      // if the LED is off turn it on and vice-versa:
      if (ledState == LOW) {
        ledState = HIGH;
      } else {
        ledState = LOW;
      }

      // set the LED with the ledState of the variable:
      digitalWrite(ledPin, ledState);
    }
  }
};

// End
