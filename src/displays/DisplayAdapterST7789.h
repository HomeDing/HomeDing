/**
 * @file DisplayAdapterST7789.h
 *
 * @brief DisplayAdapter implementation for the HomeDing library
 * adapting LCD displays using the HD44780 chip.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * -----
 * * 25.07.2018 created by Matthias Hertel
 * * 07.12.2020 no write text beyond textline end.
 */

#ifndef DisplayAdapterST7789_H
#define DisplayAdapterST7789_H

#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ST7789.h>  // Hardware-specific library for ST7789
#include <SPI.h>
#include <fonts/font10.h>
#include <fonts/font16.h>
#include <fonts/font24.h>

#include <displays/DisplayAdapter.h>

class DisplayAdapterST7789 : public DisplayAdapter {
public:
  bool start() override {
    // LOGGER_JUSTINFO("init: w:%d, h:%d, r:%d", conf->width, conf->height, conf->rotation);
    // LOGGER_JUSTINFO("  pins: l:%d, r:%d", conf->lightPin, conf->resetPin);
    // LOGGER_JUSTINFO("   spi: cs:%d, dc:%d, mosi:%d, clk:%d", conf->spiCS, conf->spiDC, conf->spiMOSI, conf->spiCLK);

    // LOGGER_JUSTINFO("Font_10: %d %d %d=%d", sizeof(Font_10), sizeof(Font_10Bitmaps), sizeof(Font_10Glyphs),
    //                 sizeof(Font_10) + sizeof(Font_10Bitmaps) + sizeof(Font_10Glyphs));

    // LOGGER_JUSTINFO("Font_16: %d %d %d=%d", sizeof(Font_16), sizeof(Font_16Bitmaps), sizeof(Font_16Glyphs),
    //                 sizeof(Font_16) + sizeof(Font_16Bitmaps) + sizeof(Font_16Glyphs));

    // LOGGER_JUSTINFO("Font_24: %d %d %d=%d", sizeof(Font_24), sizeof(Font_24Bitmaps), sizeof(Font_24Glyphs),
    //                 sizeof(Font_24) + sizeof(Font_24Bitmaps) + sizeof(Font_24Glyphs));

    display = new (std::nothrow) Adafruit_ST7789(conf->spiCS, conf->spiDC, conf->spiMOSI, conf->spiCLK, conf->resetPin);

    if (!display) {
      LOGGER_ERR("not found");
      return (false);

    } else {
      DisplayAdapter::start();

      pinMode(conf->lightPin, OUTPUT);
      digitalWrite(conf->lightPin, HIGH);

      display->init(conf->width, conf->height, SPI_MODE0);
      display->setSPISpeed(40000000);

      display->setRotation(((conf->rotation / 90) + 2) % 4);
      backColor565 = col565(backColor);
      clear();

      display->setTextWrap(false);
      _setTextHeight(16);
    }  // if
    return (true);
  };  // init()


  // virtual void setBrightness(uint8_t bright) override {
  //   display->drawBitmap((bright * 128) / 100);
  // };


  /**
   * @brief Clear all displayed information from the display.
   */
  void clear() override {
    // LOGGER_JUSTINFO("clear.");
    display->fillScreen(backColor565);
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
    display->fillRect(x, y, w, h, backColor565);
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

    _setTextHeight(h);
    display->getTextBounds(text, x, y + baseLine, &bx, &by, &bw, &bh);
    // LOGGER_JUSTINFO("     box: %d/%d w:%d h:%d", bx, by, bw, bh);

    size_t len = strlen(text);
    display->setTextColor(col565(color), backColor565);
    display->setCursor(x, y + baseLine);
    display->print(text);

    return ((bx - x) + bw);
  }  // drawText


  int drawDot(int16_t x, int16_t y, int16_t h, bool fill) override {
    // LOGGER_JUSTINFO("drawDot: (%d,%d)=%d", x, y, fill);
    int r = h / 2;

    if (fill) {
      display->fillCircle(x + r, y + r, r, col565(color)); 
    } else {
      display->drawCircle(x + r, y + r, r, col565(color));
    }
    return (1);
  };  // drawDot()


  void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1) override {
    display->drawLine(x0, y0, x1, y1, col565(color));
  }  // drawLine()

private:
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
    if (h == 0) {
      // keep
    } else if (h <= 10) {
      display->setFont(&Font_10);
      display->setTextSize(1);
      lineHeight = 10;
      baseLine = 7;
      charWidth = 8;

    } else if (h <= 16) {
      display->setFont(&Font_16);
      display->setTextSize(1);
      lineHeight = 16;
      baseLine = 12;
      charWidth = 11;

    } else if (h <= 20) {
      display->setFont(&Font_10);
      display->setTextSize(2);
      lineHeight = 20;
      baseLine = 14;
      charWidth = 16;

    } else if (h <= 24) {
      display->setFont(&Font_24);
      display->setTextSize(1);
      lineHeight = 24;  // 20+4
      baseLine = 19;
      charWidth = 14;

    } else if (h <= 32) {
      display->setFont(&Font_16);
      display->setTextSize(2);
      lineHeight = 32;
      baseLine = 24;
      charWidth = 22;
    }
  }


  /**
   * @brief Reference to the used library object
   */
  Adafruit_ST7789 *display = nullptr;

  int baseLine;  ///< baseline offset
  uint16_t backColor565;
};

#endif  // DisplayAdapterST7789_H