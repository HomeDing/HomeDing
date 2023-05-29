// DisplayTouchGT911Element.h

/**
 * @file DisplayTouchGT911Element.h
 * @brief Element capturing touch events from GT911 touch panels.
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
 * 27.05.2023 created
 *

The GT911 is a 5-Point Capacitive Touch Sensors for 7" to 8" panels.
using the I2c bus for communication.
It can deliver Input results every 40 / 10 msec.

TRhie element implements using the GT911 Touch Controller in "Normal" and "Green" mode.

 */

#include <Wire.h>
#include <TAMC_GT911.h>

#define TOUCH_GT911
#define TOUCH_SCL 32
#define TOUCH_SDA 33
#define TOUCH_INT 21
#define TOUCH_RST 25
#define TOUCH_WIDTH 320
#define TOUCH_HEIGHT 480

class DisplayTouchGT911Element : public Element {

private:
  TAMC_GT911 *tp = nullptr;

  uint16_t lastX, lastY;
  DisplayButtonElement *bFound;

  unsigned long nextRead;

public:
  /** setup the functionality. */
  void setup() override {
    Element::setup();
    // set the digital pin as output:
    tp = new TAMC_GT911(TOUCH_SDA, TOUCH_SCL, TOUCH_INT, TOUCH_RST, TOUCH_WIDTH, TOUCH_HEIGHT);

    tp->begin();
    tp->setRotation(ROTATION_INVERTED);  // ROTATION_NORMAL
    nextRead = millis() + 50;
  }

  /** run the functionality. */
  void loop() {
    Element::loop();
    unsigned long now = millis();

    if (now > nextRead) {
      tp->read();

      if (tp->isTouched) {
        // TRACE("Touch %d/%d\n", tp->points[0].x, tp->points[0].y);
        lastX = tp->points[0].x;
        lastY = tp->points[0].y;

        if (!bFound) {
          // find displaybutton at x/y
          _board->forEach("displaybutton", [this](Element *e) {
            if (!bFound) {
              DisplayButtonElement *be = (DisplayButtonElement *)e;
              if (be->touchStart(lastX, lastY)) {
                bFound = be;
              }
            }
          });

        } else {
          bool over = bFound->touchStart(lastX, lastY);
          if (!over) {
            bFound = nullptr;
          }
        }

      } else if (bFound) {
        // call touchEnd to found button
        bFound->touchEnd(lastX, lastY);
        bFound = nullptr;

      }  // if

      nextRead = millis() + 50;
    }
  }  // loop()
};

// End
