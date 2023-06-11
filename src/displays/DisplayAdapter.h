/**
 * @file DisplayAdapter.h
 *
 * @brief Abstract class, defining the functionlaity the HomeDing requires to be
 * implemented for a local attached display.
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

  DisplayAdapter() {
    color = 0x00FFFFFF;      // white
    backColor = 0x00000000;  // black
  }

  virtual bool setup(Board *b, struct DisplayConfig *c) {
    board = b;
    conf = c;
    return (true);
  }  // setup()

  /**
   * @brief start the display.
   * @return true when display is ready for operation.
   * @return false otherwise.
   */
  virtual bool start() {
    return (true);
  };

  /**
   * @brief Clear the complete display
   */
  virtual void clear(){};

  /// get height of the last drawn textline. Depends on font and text height.
  virtual int16_t getLineHeight() {
    return (lineHeight);
  };


  virtual void setBrightness(UNUSED uint8_t bright){};


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

  Board *board;
};
