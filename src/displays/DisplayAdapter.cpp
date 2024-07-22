/**
 * @file DisplayAdapter.cpp
 *
 * @brief Implementing base functionality for Display Adapters.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license.
 * See https://www.mathertel.de/License.aspx.
 *
 * Changelog:see DisplayElement.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include "DisplayAdapter.h"
#include "displays/DisplayOutputElement.h"

// tracing information (left from development for future problem analysis) can be disabled
#define TRACE(...)  // LOGGER_TRACE(__VA_ARGS__)

// tracing information on drawing (left from development for future problem analysis) can be disabled
#define TRACEDRAW(...)  // LOGGER_PRINT(__VA_ARGS__)


DisplayAdapter::DisplayAdapter() {
  color = 0x00FFFFFF;      // white
  backColor = 0x00000000;  // black
}


/// @brief setup a fresh Display Adapter
/// @param b Board Reference
/// @param c DisplayConfig Data
/// @return true
bool DisplayAdapter::setup(Board *b, struct DisplayConfig *c) {
  board = b;
  conf = c;

  displayBox.x_min = 0;
  displayBox.y_min = 0;
  if ((conf->rotation / 90) % 4 == 0) {
    displayBox.x_max = c->width;
    displayBox.y_max = c->height;
  } else {
    displayBox.x_max = c->height;
    displayBox.y_max = c->width;
  }
  return (true);
}  // setup()


/// @brief Start the display.
/// @return true when the display is ready for operation. Otherwise false.
bool DisplayAdapter::start() {
  if (conf->lightPin) {
    pinMode(conf->lightPin, OUTPUT);
    setBrightness(conf->brightness);
  }

  return (true);
};


/// @brief set brightness for panel lightning.
/// @param bright new brightness in range 0...100
void DisplayAdapter::setBrightness(uint8_t bright) {
  TRACE("setBrightness %d %d", conf->lightPin, bright);
  if (conf->lightPin) {
    bright = constrain(bright, 0, 100);
    uint32_t duty = (bright * (uint32_t)255) / 100;
    analogWrite(conf->lightPin, duty);
  }
};


/// @brief draw all DisplayOutputElements, then
/// flush all buffered pixels to the display.
bool DisplayAdapter::startFlush(bool force) {
  // TRACE("startFlush(%d, %d)", force, _needFlush);

  bool ret = false;
  if (force || _needFlush) {
    TRACE("startFlush...");

#if 1
    // direct drawing (no overlapping widgets)

    board->forEach(Element::CATEGORY::Widget, [this](Element *e) {
      DisplayOutputElement *de = (DisplayOutputElement *)e;

      TRACEDRAW("check: %d %d %d %s", de->active, de->needsDraw, de->page, de->id);

      if (de->active && de->page == page && de->needsDraw) {
        TRACEDRAW(" draw: %d/%d-%d/%d", de->box.x_min, de->box.y_min, de->box.x_max, de->box.y_max);
        // draw box with background color
        if (!(de->isOpaque)) {
          drawRectangle(de->box, RGB_TRANSPARENT, conf->backgroundColor);
        }
        de->draw();
        de->needsDraw = false;
      }
    });

#else
    // find all to be redrawn elements.
    BoundingBox box;

    // find extended bounding box of all un-drawn elements
    board->forEach(Element::CATEGORY::Widget, [this, &box](Element *e) {
      DisplayOutputElement *de = (DisplayOutputElement *)e;

      TRACEDRAW("check: %d %d %d %s", de->active, de->needsDraw, de->page, de->id);

      if (de->active && de->page == page && de->needsDraw) {
        TRACEDRAW("  box: %d/%d-%d/%d", de->box.x_min, de->box.y_min, de->box.x_max, de->box.y_max);
        box.extend(de->box);
      }
    });

    TRACEDRAW("box = %d/%d-%d/%d", box.x_min, box.y_min, box.x_max, box.y_max);

    // draw box with background color
    drawRectangle(box, RGB_TRANSPARENT, conf->backgroundColor);

    // draw all inside box
    board->forEach(Element::CATEGORY::Widget, [this, &box](Element *e) {
      DisplayOutputElement *de = (DisplayOutputElement *)e;

      TRACEDRAW("draw: %s %d", de->id, de->page);
      if (de->active && de->page == page) {
        if (box.overlaps(de->box)) {
          TRACEDRAW("draw...");
          de->draw();
          de->needsDraw = false;
        }
      }
    });
#endif

    // send to the device
    this->flush();
    ret = true;
  }
  _needFlush = false;
  return (ret);
};
