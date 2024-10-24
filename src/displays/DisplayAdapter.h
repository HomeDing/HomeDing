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

#pragma once

#include <Wire.h>
#include <WireUtils.h>

#include <Board.h>
#include <displays/DisplayElement.h>

#define MAX_DISPLAY_STRING_LEN 80

class DisplayAdapter {
public:
  virtual ~DisplayAdapter() = default;

  DisplayAdapter();

  /// @brief setup a fresh Display Adapter
  /// @param b Board Reference
  /// @param c HomeDing::DisplayConfig Configuration Data
  /// @return true
  virtual bool setup(Board *b);

  /// @brief Start the display.
  /// @return true when the display is ready for operation. Otherwise false.
  virtual bool start();

  /// get height of the last drawn textline. Depends on font and text height.
  virtual int16_t getLineHeight() {
    return (lineHeight);
  };


  // Bounding box of the drawable area.
  BoundingBox displayBox;


  /// When the display supports overlapping elements the drawing must not be immediate.
  /// The flush() command will start  drawing all elements ant the covered elements.
  bool deferDrawing() {
    return (true);
  };


  /// @brief set brightness for panel lightning.
  /// @param bright new brightness in range 0...100
  virtual void setBrightness(uint8_t bright);


  /// @brief Set default draw color
  /// @param col The 32-bit draw color in 0x00rrggbb.
  virtual void setColor(const uint32_t col) {
    color = col;
  };


  /// @brief Get default draw color.
  /// @return The default draw color.
  virtual uint32_t getColor() {
    return (color);
  };

  /// @brief Set default background color
  /// @param col The 32-bit background color in 0x00rrggbb.
  virtual void setBackgroundColor(const uint32_t col) {
    backColor = col;
  };

  /// @brief Set default border color
  /// @param col The 32-bit border color in 0x00rrggbb.
  virtual void setBorderColor(const uint32_t col) {
    borderColor = col;
  };

  /// @brief Clear the complete display
  virtual void clear() {
    _needFlush = true;
  };

  virtual int drawText(int16_t x, int16_t y, int16_t h, String &text) {
    return (drawText(x, y, h, text.c_str()));
  };

  virtual int drawText(int16_t /* x */, int16_t /* y */, int16_t /* h */, const char *text) {
    _needFlush = true;
    return (charWidth * strnlen(text, MAX_DISPLAY_STRING_LEN));
  };

  virtual void drawPixel(int16_t /* x */, int16_t /* y */, uint32_t /* color */) {
    _needFlush = true;
  };

  virtual void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
    drawLine(x0, y0, x1, y1, color);
  };

  virtual void drawLine(int16_t /* x0 */, int16_t /* y0 */, int16_t /* x1 */, int16_t /* y1 */, uint32_t /* color */) {
    _needFlush = true;
  };

  virtual void drawButton(int16_t /* x */, int16_t /* y */, int16_t /* w */, int16_t /* h */, const char * /* text */, bool /* pressed = false */) {
    _needFlush = true;
  };

  virtual void drawRectangle(BoundingBox & /* box */, uint32_t /* borderColor */, uint32_t /* fillColor = RGB_UNDEFINED */) {
    _needFlush = true;
  };

  virtual void drawCircle(BoundingBox & /* box */, uint32_t /* borderColor */, uint32_t /* fillColor = RGB_UNDEFINED */) {
    _needFlush = true;
  };

  virtual uint32_t getPixel(int16_t /* x */, int16_t /* y */) {
    return(HomeDing::displayConfig.backgroundColor);
  };

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
  int16_t lineHeight;  ///< total height of a text line
  int16_t charWidth;   ///< width of a character

  uint32_t color;        ///< default draw color
  uint32_t backColor;    ///< default background color
  uint32_t borderColor;  ///< default border color

  uint8_t _lightChannel;

  /// @brief  the display buffer is not in sync with the display.
  bool _needFlush;

  /// @brief after buffered pixels have been sent to the display, clear needSync flag.
  virtual void flush() {
    _needFlush = false;
  };

  Board *board;
};
