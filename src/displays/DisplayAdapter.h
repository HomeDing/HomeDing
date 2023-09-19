/**
 * @file DisplayAdapter.h
 *
 * @brief Implementing base functionaliy for Display Adapters.
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
  /// @param c DisplayConfig Data
  /// @return true
  virtual bool setup(Board *b, struct DisplayConfig *c);

  /// @brief Start the display.
  /// @return true when the display is ready for operation. Otherwise false.
  virtual bool start();

  /**
   * @brief Clear the complete display
   */
  virtual void clear(){};

  /// get height of the last drawn textline. Depends on font and text height.
  virtual int16_t getLineHeight() {
    return (lineHeight);
  };


  /// @brief set brightness for panel lightng.
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

  virtual uint32_t getBackgroundColor() {
    return (backColor);
  };

  /**
   * @brief Clear a position or region.
   * @param x
   * @param y
   * @param w
   * @param h
   */
  virtual void clear(UNUSED int16_t x, UNUSED int16_t y, UNUSED int16_t w, UNUSED int16_t h){};

  virtual int drawText(int16_t x, int16_t y, int16_t h, String &text) {
    return (drawText(x, y, h, text.c_str()));
  };

  virtual int drawText(UNUSED int16_t x, UNUSED int16_t y, UNUSED int16_t h, const char *text) {
    return (charWidth * strnlen(text, MAX_DISPLAY_STRING_LEN));
  };

  virtual void drawLine(UNUSED int16_t x0, UNUSED int16_t y0, UNUSED int16_t x1, UNUSED int16_t y1){};

  virtual void drawButton(UNUSED int16_t x, UNUSED int16_t y, UNUSED int16_t w, UNUSED int16_t h, UNUSED const char *text, UNUSED bool pressed = false){};

  virtual int drawDot(UNUSED int16_t x, UNUSED int16_t y, int16_t h, UNUSED bool fill) {
    return (h);
  };

  /**
   * @brief The flush method must be called after every output sequence to allow
   * combined sending new information to the display.
   */
  virtual void flush(){};

  /**
   * @brief current displayed page
   */
  int page = 1;

  /**
   * @brief max used page
   */
  int maxpage = 1;

protected:
  struct DisplayConfig *conf;

  int16_t lineHeight;  ///< total height of a text line
  int16_t charWidth;   ///< width of a character

  uint32_t color;      ///< default draw color
  uint32_t backColor;  ///< default background color
  uint32_t borderColor;  ///< default border color

  uint8_t _lightChannel;

  Board *board;
};
