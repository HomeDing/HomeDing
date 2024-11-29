/**
 * @file DisplayAdapter.h
 *
 * @brief Implementing base functionality for Display Adapters.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license.
 * See https://www.mathertel.de/License.aspx.
 * More information on https://www.mathertel.de/Arduino
 * -----
 * 25.07.2018 created by Matthias Hertel
 * 23.09.2018 flush() method added.
 * 22.07.2023 cpp file added (from DisplayAdapter.h).
 *            handling lightPin and brightness.
 * 19.02.2024 startFlush(bool) method added.
 */

/*
 The display adapters are acting as a connecting interface decoupling the HomeDing library implementation
 from underlying GFX implementations (chip driver and bus).

 After initialization the adapter can be used for drawing by using 2 possible procedures:

 Drawing pixels on the display by using the lo-level functions

 * `startWrite()` -- to start a pixel sequence
 * `writePixel(...)` -- draw pixels with a color, be sure not to draw outside the display limits.
 * `endWrite()` -- to stop a pixel sequence

 Drawing simple functions

 * `drawNNNN(...)` -- draw the whole primitive by using GFX functionality.

Some drivers capture everything in an internal buffer and will transfer it at a whole after drawing.
Therefore after completing any drawing the flush() method must be called to transfer the pixels to the board.

* `flush()` -- finalize drawing by updating the display.

*/

#pragma once

#include <Wire.h>
#include <WireUtils.h>

#include <Board.h>
#include <displays/DisplayElement.h>

#define MAX_DISPLAY_STRING_LEN 80

class DisplayAdapter {
public:
  virtual ~DisplayAdapter() = default;

  /// @brief setup a fresh Display Adapter
  /// @param b Board Reference
  /// @param c HomeDing::DisplayConfig Configuration Data
  /// @return true
  virtual bool setup(Board *b);

  /// @brief Start the display using the HomeDing::displayConfig settings.
  /// @return true when the display is ready for operation. Otherwise false.
  virtual bool start();

  /// @brief Clear the complete display
  virtual void clear();

  /// @brief Fill a rectangle area with one color or clear text.
  /// @param x Top left corner x coordinate.
  /// @param y Top left corner y coordinate.
  /// @param w Width in pixels or text characters.
  /// @param h Height in pixels or text characters.
  /// @param color color for filling for pixel displays.
  /// For text only displays this function should clear the text of the given range.
  virtual void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color);


  /// @brief Fill a rectangle area with one color or clear text.
  /// @param box Bounding Box of the rectangle.
  /// @param color color for filling for pixel displays.
  void fillRect(BoundingBox &box, uint32_t color);


  /// @brief calculate the Bounding box of a text drawn at 0/0
  /// @param h Text heigth
  /// @param text sample text
  /// @return size of the resulting box when character size is 1
  virtual BoundingBox textBox(int16_t h, const char *text) {
    (void)h;
    BoundingBox b(0, 0, strlen(text) - 1, 0);
    return (b);
  };


  /// @brief print a text at the specified position
  /// @param x x position
  /// @param y y position
  /// @param h character height
  /// @param text text to be printed.
  /// @return Bounding Box of the drawn text.
  virtual BoundingBox drawText(int16_t x, int16_t y, int16_t h, const char *text, uint32_t color);

  // Bounding box of the drawable area.
  BoundingBox displayBox;

  /// @brief set brightness for panel lightning.
  /// @param bright new brightness in range 0...100
  virtual void setBrightness(uint8_t bright);


  // ===== low level pixel drawing and reading =====

  virtual void startWrite() {}

  virtual void writePixel(int16_t x, int16_t y, uint32_t color) {
    (void)x;
    (void)y;
    (void)color;
  };

  virtual void endWrite() {
    _needFlush = true;
  }

  virtual uint32_t getPixel(int16_t /* x */, int16_t /* y */) {
    return (HomeDing::displayConfig.backgroundColor);
  };


  // ===== supporting deferred drawing and flushing =====

  // @brief remember that flush is required after sequence.
  virtual void setFlush() {
    // LOGGER_TRACE("setFlush()");
    _needFlush = true;
  };


  /// @brief draw all DisplayOutputElements, then
  /// flush all buffered pixels to the display.
  bool startFlush(bool force);


  /// * @brief current displayed page
  int page = 1;

  /// @brief max used page
  int maxpage = 1;

protected:
  /// @brief  the display buffer is not in sync with the display.
  bool _needFlush;

  /// @brief after buffered pixels have been sent to the display, clear needSync flag.
  virtual void flush() {
    _needFlush = false;
  };

  Board *board;
};


// ===== static variables for the display in the HomeDing namespace

namespace HomeDing {
extern DisplayAdapter *displayAdapter;

extern uint32_t strokeColor;
extern uint32_t fillColor;

extern void stroke(int16_t x, int16_t y);
extern void fill(int16_t x, int16_t y);
extern void draw(int16_t x, int16_t y, uint32_t color);
}
