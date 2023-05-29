/**
 * @file DisplayAGFXAdapter.h
 *
 * @brief DisplayAdapter implementation for the HomeDing library
 * adapting TFT displays using the Arduino_GFX library.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * -----
 * * 27.05.2023 created by Matthias Hertel
 */

#pragma once

#include <Arduino_GFX_Library.h>

#include <fonts/font10.h>
#include <fonts/font16.h>
#include <fonts/font24.h>

#define PANELTRACE(...) // Serial.printf(__VA_ARGS__)

class DisplayAGFXAdapter : public DisplayAdapter {
public:
  ~DisplayAGFXAdapter() = default;

  bool start() override {
    PANELTRACE("init: w:%d, h:%d, r:%d\n", conf->width, conf->height, conf->rotation);
    PANELTRACE(" col: #%08x / #%08x\n", conf->drawColor, conf->backgroundColor);
    // LOGGER_JUSTINFO("  pins: l:%d, r:%d", conf->lightPin, conf->resetPin);
    // LOGGER_JUSTINFO("   i2c: adr:%d, sda:%d, scl:%d", conf->i2cAddress, conf->i2cSDA, conf->i2cSCL);
    // LOGGER_JUSTINFO("   spi: cs:%d, dc:%d, mosi:%d, clk:%d", conf->spiCS, conf->spiDC, conf->spiMOSI, conf->spiCLK);

    // LOGGER_JUSTINFO("Font_10: %d %d %d=%d", sizeof(Font_10), sizeof(Font_10Bitmaps), sizeof(Font_10Glyphs),
    //                 sizeof(Font_10) + sizeof(Font_10Bitmaps) + sizeof(Font_10Glyphs));

    // LOGGER_JUSTINFO("Font_16: %d %d %d=%d", sizeof(Font_16), sizeof(Font_16Bitmaps), sizeof(Font_16Glyphs),
    //                 sizeof(Font_16) + sizeof(Font_16Bitmaps) + sizeof(Font_16Glyphs));

    // LOGGER_JUSTINFO("Font_24: %d %d %d=%d", sizeof(Font_24), sizeof(Font_24Bitmaps), sizeof(Font_24Glyphs),
    //                 sizeof(Font_24) + sizeof(Font_24Bitmaps) + sizeof(Font_24Glyphs));

    if (!gfx) {
      LOGGER_ERR("not found");
      return (false);

    } else {
      gfx->begin();

      DisplayAdapter::start();

      if (conf->lightPin >= 0) {
        pinMode(conf->lightPin, OUTPUT);
        digitalWrite(conf->lightPin, HIGH);
      }  // if

      gfx->setTextWrap(false);
      setColor(conf->drawColor);
      setBackgroundColor(conf->backgroundColor);
      _setTextHeight(conf->height > 64 ? 16 : 8);
      clear();
      flush();
    }  // if

    return (true);
  };  // init()


  virtual void setColor(const uint32_t col) override {
    DisplayAdapter::setColor(col);
    drawColor565 = col565(col);
  };

  virtual uint16_t getColor565() {
    return (drawColor565);
  };

  virtual void setBackgroundColor(const uint32_t col) override {
    PANELTRACE("setBackgroundColor #%08x\n", col);
    DisplayAdapter::setBackgroundColor(col);
    backColor565 = col565(col);
  };

  virtual uint16_t getBackgroundColor565() {
    return (backColor565);
  };

  /**
   * @brief Clear all displayed information from the display.
   */
  void clear() override {
    PANELTRACE("clear #%08x\n", conf->backgroundColor);
    gfx->fillScreen(col565(conf->backgroundColor));
  };  // clear()


  /**
   * @brief fill rectangle with current background color.
   * @param x x-position or offset of the text.
   * @param y y-position of the area.
   * @param w width of the area.
   * @param h height of the area, assumed always 1.
   */
  void clear(int16_t x, int16_t y, int16_t w, int16_t h) override {
    // PANELTRACE("clear: %d %d %d %d\n", x, y, w, h);
    gfx->fillRect(x, y, w, h, backColor565);
  };  // clear()

  /**
   * @brief Draw a text at this position using the specific height.-
   * @param x x-position or offset of the text.
   * @param y y-position of the text.
   * @param size height of the characters box.
   * @param text the text.
   */
  int drawText(int16_t x, int16_t y, int16_t h, const char *text) override {
    // PANELTRACE("drawText: %d/%d h:%d t:<%s>\n", x, y, h, text);
    // PANELTRACE("  colors: %d on %d\n", drawColor565, backColor565);

    // textbox dimensions
    int16_t bx, by;
    uint16_t bw, bh;

    _setTextHeight(h);
    gfx->getTextBounds(text, x, y + baseLine, &bx, &by, &bw, &bh);
    // PANELTRACE("     box: %d/%d w:%d h:%d", bx, by, bw, bh);

    gfx->setTextColor(drawColor565, backColor565);
    gfx->setCursor(x, y + baseLine);
    gfx->print(text);

    return ((bx - x) + bw);
  }  // drawText


  /// @brief Draw a button
  /// @param x
  /// @param y
  /// @param w
  /// @param h
  /// @param text
  virtual void drawButton(int16_t x, int16_t y, int16_t w, int16_t h, const char *text, bool pressed = false) override {
    // LOGGER_JUSTINFO("drawButton: (%d,%d,%d,%d) <%s> %d\n", x, y, w, h, text, pressed);
    const uint16_t paddingVertical = 4;

    // textbox dimensions
    int16_t bx, by;
    uint16_t bw, bh;

    _setTextHeight(h - (2 * paddingVertical));
    gfx->getTextBounds(text, x, y + baseLine, &bx, &by, &bw, &bh);
    // LOGGER_JUSTINFO("  b(%d,%d,%d,%d)", bx, by, bw, bh);

    // calculate textbox offset
    int16_t dx = (w - bw) / 2;
    int16_t dy = (h - lineHeight) / 2 - 1;
    // LOGGER_JUSTINFO("  +(%d,%d)", dx, dy);

    uint16_t fCol = (pressed ? backColor565 : drawColor565);
    uint16_t bCol = (pressed ? drawColor565 : backColor565);
    // LOGGER_JUSTINFO("  c(#%08x,#%08x)", fCol, bCol);

    gfx->fillRoundRect(x, y, w, h, paddingVertical + 1, bCol);
    gfx->drawRoundRect(x, y, w, h, paddingVertical + 1, fCol);

    gfx->setTextColor(fCol, bCol);
    gfx->setCursor(x + dx, y + dy + baseLine);
    gfx->print(text);
  };

  /// @brief Draw a boolean indicator on/off
  /// @param x X Position
  /// @param y Y Position
  /// @param h Height of the indicator
  /// @param fill 
  /// @return 
  int drawDot(int16_t x, int16_t y, int16_t h, bool fill) override {
    // LOGGER_JUSTINFO("drawDot: (%d,%d)=%d", x, y, fill);
    int r = h / 2;

    if (fill) {
      gfx->fillCircle(x + r, y + r, r, drawColor565);
    } else {
      gfx->drawCircle(x + r, y + r, r, drawColor565);
    }
    return (1);
  };  // drawDot()


  void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1) override {
    PANELTRACE("drawLine(%d/%d - %d/%d)\n", x0, y0, x1, y1);
    gfx->drawLine(x0, y0, x1, y1, drawColor565);
  }  // drawLine()


protected:
  // convert 32-bit color to 16-bit color
  // 0x00rrggbb to rrrrrggggggbbbbb
  // use gfx->color565 ???
  uint16_t col565(uint32_t color) {

    uint16_t col =
      ((color & 0x00F80000) >> 8)
      | ((color & 0x0000FC00) >> 5)
      | ((color & 0x000000F8) >> 3);
    return (col);
  }


  /** Set max. height of text in a box */
  void _setTextHeight(int16_t h) {
    // PANELTRACE("_setTextHeight(%d)\n", h);
    int16_t base, fit;

    // 8, 10, 16, 24

    if (h > 0) {
      base = 8;
      fit = h % base;

      if ((h >= 10) && (h % 10 <= fit)) {
        base = 10;
        fit = h % base;
      }

      if ((h >= 16) && (h % 16 <= fit)) {
        base = 16;
        fit = h % base;
      }

      if ((h >= 24) && (h % 24 <= fit)) {
        base = 24;
        fit = h % base;
      }

      loadFont(base, h / base);
      PANELTRACE(" >(%d,%d)\n", lineHeight, baseLine);
    }
  }

  // load a builtin font
  void loadFont(int16_t height, int8_t factor = 1) {
    // PANELTRACE("loadFont(%d, %d)\n", height, factor);
    const GFXfont *font = nullptr;

    if (height <= 8) {
      // builtin 8pt font
      lineHeight = 8;
      baseLine = 0;
      charWidth = 6;

    } else if (height <= 10) {
      font = &Font_10;
      lineHeight = 10;
      baseLine = 7;
      charWidth = 8;

    } else if (height <= 16) {
      font = &Font_16;
      lineHeight = 16;
      baseLine = 12;
      charWidth = 11;

    } else if (height <= 24) {
      font = &Font_24;
      lineHeight = 24;  // 20+4
      baseLine = 19;
      charWidth = 14;
    }  // if

    gfx->setFont(font);
    gfx->setTextSize(factor);
    lineHeight *= factor;
    baseLine *= factor;
    charWidth *= factor;
  }  // loadFont()

  /**
   * @brief Reference to the used library object
   */
  Arduino_DataBus *bus = nullptr;
  Arduino_GFX *gfx = nullptr;


  int baseLine;  ///< baseline offset
  uint16_t backColor565;
  uint16_t drawColor565;
};

