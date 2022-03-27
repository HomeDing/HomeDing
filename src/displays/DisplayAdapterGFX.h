/**
 * @file DisplayAdapterGFX.h
 *
 * @brief DisplayAdapter implementation for the HomeDing library
 * Base class implementation for using AdaFruit GFX based display drivers.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * -----
 * * 18.03.2022 created by Matthias Hertel
 */

#ifndef DisplayAdapterGFX_H
#define DisplayAdapterGFX_H

#include <Adafruit_GFX.h>  // Core graphics library

#include <fonts/font10.h>
#include <fonts/font16.h>
#include <fonts/font24.h>

#include <displays/DisplayAdapter.h>

class DisplayAdapterGFX : public DisplayAdapter {
public:

  ~DisplayAdapterGFX() = default;

  bool start() override {
    // LOGGER_JUSTINFO("init: w:%d, h:%d, r:%d", conf->width, conf->height, conf->rotation);
    // LOGGER_JUSTINFO("  pins: l:%d, r:%d", conf->lightPin, conf->resetPin);
    // LOGGER_JUSTINFO("   i2c: adr:%d, sda:%d, scl:%d", conf->i2cAddress, conf->i2cSDA, conf->i2cSCL);
    // LOGGER_JUSTINFO("   spi: cs:%d, dc:%d, mosi:%d, clk:%d", conf->spiCS, conf->spiDC, conf->spiMOSI, conf->spiCLK);

    // LOGGER_JUSTINFO("Font_10: %d %d %d=%d", sizeof(Font_10), sizeof(Font_10Bitmaps), sizeof(Font_10Glyphs),
    //                 sizeof(Font_10) + sizeof(Font_10Bitmaps) + sizeof(Font_10Glyphs));

    // LOGGER_JUSTINFO("Font_16: %d %d %d=%d", sizeof(Font_16), sizeof(Font_16Bitmaps), sizeof(Font_16Glyphs),
    //                 sizeof(Font_16) + sizeof(Font_16Bitmaps) + sizeof(Font_16Glyphs));

    // LOGGER_JUSTINFO("Font_24: %d %d %d=%d", sizeof(Font_24), sizeof(Font_24Bitmaps), sizeof(Font_24Glyphs),
    //                 sizeof(Font_24) + sizeof(Font_24Bitmaps) + sizeof(Font_24Glyphs));

    if (!gfxDisplay) {
      LOGGER_ERR("GFX display not available");

    } else {
      DisplayAdapter::start();

      if (conf->lightPin > 0) {
        pinMode(conf->lightPin, OUTPUT);
        digitalWrite(conf->lightPin, HIGH);
      }  // if

      gfxDisplay->setTextWrap(false);
      gfxDisplay->cp437(true);
      gfxDisplay->setRotation((conf->rotation / 90) % 4);
      _setTextHeight(8);
      clear();
      flush();
    }  // if
    return (true);
  };  // init()


  // virtual void setBrightness(uint8_t bright) override {
  //   gfxDisplaydrawBitmap((bright * 128) / 100);
  // };


  /**
   * @brief Clear all displayed information from the display.
   */
  void clear() override {
    // LOGGER_JUSTINFO("clear.");
    gfxDisplay->fillScreen(backColor565);
  };  // clear()


  /**
   * @brief Clear information from the display in this area.
   * @param x x-position or offset of the text.
   * @param y y-position of the area.
   * @param w width of the area.
   * @param h height of the area, assumed always 1.
   */
  void clear(int16_t x, int16_t y, int16_t w, int16_t h) override {
    // LOGGER_JUSTINFO("clear: %d %d %d %d", x, y, w, h);
    gfxDisplay->fillRect(x, y, w, h, backColor565);
  };  // clear()


  /**
   * @brief Draw a text at this position using the specific height.-
   * @param x x-position or offset of the text.
   * @param y y-position of the text.
   * @param size height of the characters box.
   * @param text the text.
   */
  int drawText(int16_t x, int16_t y, int16_t h, const char *text) override {
    int16_t bx, by;
    uint16_t bw, bh;
    // LOGGER_JUSTINFO("drawText: %d/%d h:%d t:<%s>", x, y, h, text);
    // LOGGER_JUSTINFO("  colors: %d on %d", drawColor565, backColor565);

    _setTextHeight(h);
    gfxDisplay->getTextBounds(text, x, y + baseLine, &bx, &by, &bw, &bh);
    // LOGGER_JUSTINFO("     box: %d/%d w:%d h:%d", bx, by, bw, bh);

    gfxDisplay->setTextColor(drawColor565, backColor565);
    gfxDisplay->setCursor(x, y + baseLine);
    gfxDisplay->print(text);

    return ((bx - x) + bw);
  }  // drawText


  int drawDot(int16_t x, int16_t y, int16_t h, bool fill) override {
    // LOGGER_JUSTINFO("drawDot: (%d,%d)=%d", x, y, fill);
    int r = h / 2;

    if (fill) {
      gfxDisplay->fillCircle(x + r, y + r, r, drawColor565);
    } else {
      gfxDisplay->drawCircle(x + r, y + r, r, drawColor565);
    }
    return (1);
  };  // drawDot()


  void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1) override {
    gfxDisplay->drawLine(x0, y0, x1, y1, drawColor565);
  }  // drawLine()

  virtual void setColor(uint32_t col) override {
    DisplayAdapter::setColor(col);
    drawColor565 = col565(col);
    // LOGGER_JUSTINFO("setColor: (%08x)=%08x", col, drawColor565);
  };

protected:
  /**
   * @brief Reference to the GFX display
   */
  Adafruit_GFX *gfxDisplay = nullptr;

  // convert 32-bit color to 16-bit color
  // 0x00RRGGBB to RRRRRGGGGGGBBBBB
  uint16_t col565(uint32_t color) {

    uint16_t col =
      ((color & 0x00F80000) >> 8)
      | ((color & 0x0000FC00) >> 5)
      | ((color & 0x000000F8) >> 3);

    return (col);
  }


  /** Set max. height of text in a box */
  void _setTextHeight(int16_t h) {
    // LOGGER_JUSTINFO("textHeight: %d", h);

    if (h == 0) {
      // keep
    } else if (h <= 8) {
      gfxDisplay->setFont();
      gfxDisplay->setTextSize(1);
      lineHeight = 8;
      baseLine = 0;
      charWidth = 6;

    } else if (h <= 10) {
      gfxDisplay->setFont(&Font_10);
      gfxDisplay->setTextSize(1);
      lineHeight = 10;
      baseLine = 7;
      charWidth = 8;

    } else if (h <= 16) {
      gfxDisplay->setFont(&Font_16);
      gfxDisplay->setTextSize(1);
      lineHeight = 16;
      baseLine = 12;
      charWidth = 11;

    } else if (h <= 20) {
      gfxDisplay->setFont(&Font_10);
      gfxDisplay->setTextSize(2);
      lineHeight = 20;
      baseLine = 14;
      charWidth = 16;

    } else if (h <= 24) {
      gfxDisplay->setFont(&Font_24);
      gfxDisplay->setTextSize(1);
      lineHeight = 24;  // 20+4
      baseLine = 19;
      charWidth = 14;

    } else if (h <= 32) {
      gfxDisplay->setFont(&Font_16);
      gfxDisplay->setTextSize(2);
      lineHeight = 32;
      baseLine = 24;
      charWidth = 22;
    }
  }

  int baseLine;  ///< baseline offset
  uint16_t backColor565;
  uint16_t drawColor565;
};

#endif  // DisplayAdapterGFX_H