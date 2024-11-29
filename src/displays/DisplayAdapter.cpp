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

// enable TRACE to create detailed output
#define TRACE(...)  // LOGGER_TRACE(__VA_ARGS__)

// tracing information on drawing (left from development for future problem analysis) can be disabled
#define TRACEDRAW(...)  // LOGGER_PRINT(__VA_ARGS__)

namespace HomeDing {

DisplayAdapter *displayAdapter = nullptr;

uint32_t strokeColor;
uint32_t fillColor;

void stroke(int16_t x, int16_t y) {
  displayAdapter->writePixel(x, y, strokeColor);
}

void fill(int16_t x, int16_t y) {
  displayAdapter->writePixel(x, y, fillColor);
}

void draw(int16_t x, int16_t y, uint32_t color) {
  displayAdapter->writePixel(x, y, color);
}

}


using namespace HomeDing;

/// @brief setup a fresh Display Adapter
/// @param b Board Reference
/// @param c HomeDing::DisplayConfig Configuration Data
/// @return true
bool DisplayAdapter::setup(Board *b) {
  board = b;

  displayBox.x_min = 0;
  displayBox.y_min = 0;

  if ((displayConfig.rotation / 90) % 4 == 0) {
    displayBox.x_max = displayConfig.width;
    displayBox.y_max = displayConfig.height;
  } else {
    displayBox.x_max = displayConfig.height;
    displayBox.y_max = displayConfig.width;
  }
  return (true);
}  // setup()


/// @brief Start the display.
/// @return true when the display is ready for operation. Otherwise false.
bool DisplayAdapter::start() {
  if (displayConfig.lightPin >= 0) {
    pinMode(displayConfig.lightPin, OUTPUT);
    setBrightness(displayConfig.brightness);
  }
  return (true);
};


/// @brief Clear the complete display
void DisplayAdapter::clear() {
  fillRect(displayBox, displayConfig.backgroundColor);
};


/// @brief Clear a specific rectangle area
void DisplayAdapter::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color) {
  _needFlush = true;
};


/// @brief Clear a specific rectangle area
void DisplayAdapter::fillRect(BoundingBox &box, uint32_t color) {
  fillRect(box.x_min, box.y_min, box.width(), box.height(), color);
};


BoundingBox DisplayAdapter::drawText(int16_t x, int16_t y, int16_t h, const char *text, uint32_t color) {
  BoundingBox b(x, y, x + strlen(text) - 1, y);
  _needFlush = true;
  return (b);
};


/// @brief set brightness for panel lightning.
/// @param bright new brightness in range 0...100
void DisplayAdapter::setBrightness(uint8_t bright) {
  TRACE("setBrightness %d %d", displayConfig.lightPin, bright);
  if (displayConfig.lightPin >= 0) {
    bright = (bright > 100) ? 100 : bright;
    uint32_t duty = (bright * (uint32_t)255) / 100;
    analogWrite(displayConfig.lightPin, duty);
  }
};


/// @brief draw all DisplayOutputElements, then
/// flush all buffered pixels to the display.

bool DisplayAdapter::startFlush(bool force) {
  // TRACEDRAW("startFlush(%d, %d)", force, _needFlush);

  bool ret = false;
  if (force || _needFlush) {
    TRACEDRAW(" startFlush...");

#if 1
    // direct drawing (no overlapping widgets)
    // all opaque display output element need drawing the background

    board->forEach(Element::CATEGORY::Widget, [this](Element *e) {
      DisplayOutputElement *de = (DisplayOutputElement *)e;

      TRACEDRAW(" check: %d %d %d %s", de->active, de->needsDraw, de->page, de->id);

      if (de->active && de->page == page && de->needsDraw) {
        TRACEDRAW(" draw: %d/%d-%d/%d", de->box.x_min, de->box.y_min, de->box.x_max, de->box.y_max);

        if (!(de->isOpaque)) {
          // The DisplayOutputElement needs drawing the background
          fillRect(de->box, displayConfig.backgroundColor);
          // fillRect(de->box.x_min, de->box.y_min, de->box.x_max - de->box.x_min + 1, de->box.y_max - de->box.y_min + 1, displayConfig.backgroundColor);
        }
        de->draw();
        de->needsDraw = false;  // done.
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
    drawRectangle(box, RGB_TRANSPARENT, displayConfig.backgroundColor);

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
  return (ret);
};
