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

  bool wasTouched = false;
  uint16_t wasX, wasY;
  DisplayButtonElement *bFound;

  unsigned long nextRead;

  // send a touchStart to all button elements until a resonsive button was found
  void _sendTouchStart(uint16_t x, uint16_t y) {
    // LOGGER_EINFO("_sendTouchStart(%d/%d)", x, y);
    bFound = nullptr;

    // redraw all display elements
    _board->forEach("displaybutton", [this, x, y](Element *e) {
      if (!bFound) {
        DisplayButtonElement *be = (DisplayButtonElement *)e;
        if (be->touchStart(x, y)) bFound = be;
      }
    });
    if (bFound) {
      // LOGGER_EINFO("btn (%s)", bFound->id);
    }
  } // _sendTouchStart


  // call touchEnd to found button
  void _sendTouchEnd(uint16_t x, uint16_t y) {
    // LOGGER_EINFO("_sendTouchEnd(%d/%d)", x, y);
    if (bFound) {
      bFound->touchEnd(x, y);
      bFound = nullptr;
    }
  }  // _sendTouchEnd

public:
  /** setup the functionality. */
  void
  setup() override {
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
        if (tp->touches > 8) {
          Serial.printf("Touch-Reset\n");
          tp->reset();
        }

        if (wasTouched) {
          // maybe a gesture...

        } else {
          for (int i = 0; i < tp->touches; i++) {
            // Serial.printf("Touch-%d %d/%d\n", i, tp->points[i].x, tp->points[i].y);
            wasX = tp->points[i].x;
            wasY = tp->points[i].y;
            wasTouched = true;
            _sendTouchStart(wasX, wasY);
          }
        }

      } else if (wasTouched) {
        // touchEnd();
        _sendTouchEnd(wasX, wasY);
        wasTouched = false;
      }
      nextRead = millis() + 50;
    }
  }
};

// End
