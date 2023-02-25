/**
 * @file MyElement03.cpp
 * @brief advanced Element implementation with configuration and lifecycle control.
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

#include "MyElement-03.h"

/** static factory function to create a new MyElement03 */
Element *MyElement03::create()
{
  return (new MyElement03());
} // create()


void MyElement03::setup() {
  // call the default setup() on the base class
  Element::setup();

  // set the digital pin as output:
  pinMode(ledPin, OUTPUT);
}


/** Set a parameter or property to a new value or start an action. */
bool MyElement03::set(const char *name, const char *value) {
  bool ret = Element::set(name, value);

  if (_stricmp(name, "pin") == 0) {
    ledPin = _atopin(value);
    ret = true;
  } else if (_stricmp(name, "interval") == 0) {
    interval = _scanDuration(value) / 2;
    ret = true;
  }
  return (ret);
};


// start/activate the element
void MyElement03::start() {
  // call the default start() on the base class
  Element::start();

  // turn LED ON and start new cycle.
  ledState = LOW;
  digitalWrite(ledPin, ledState);
  previousMillis = millis();
}


/** run the functionality. */
void MyElement03::loop() {
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


// stop/terminate the element
void MyElement03::term() {
  // call the default term() on the base class
  Element::term();

  // turn LED off.
  ledState = HIGH;
  digitalWrite(ledPin, ledState);
}

bool MyElement03::registered = ElementRegistry::registerElement("my", MyElement03::create);

// End
