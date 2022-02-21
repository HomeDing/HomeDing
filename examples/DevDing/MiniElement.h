/**
 * @file MiniElement.ino
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

// #include <Arduino.h>
// #include <HomeDing.h>

// Board homeding;

class MiniElement : public Element {

  unsigned long lastTime;

  /**
   * @brief setup the functionality.
   */
  void setup() {
    // Setup code here...
    lastTime = millis();

    Serial.println("Setup done.");
  }

  /**
   * @brief run the functionality.
   */
  void loop() {
    if (millis() > lastTime + 4000) {
      Serial.println("Blink..");
      lastTime = millis();
    }
  }
};

// End
