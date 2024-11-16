/**
 * @file DisplayAGFXAdapter.h
 *
 * @brief DisplayAdapter implementation for the HomeDing library
 * for displays using the Arduino_GFX library.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * -----
 * * 27.05.2023 created by Matthias Hertel
 */

#pragma once

#define UNUSED(expr) \
  do { (void)(expr); } while (0)

#include <Arduino_GFX_Library.h>
#include <gfxDraw.h>

#include <displays/DisplayAdapter.h>

using namespace HomeDing;

#if defined(ESP8266)
#include <pgmspace.h>
#undef PROGMEM
#define PROGMEM STORE_ATTR

#elif !defined(PROGMEM)
#define PROGMEM

#endif

#include <fonts/font.h>

#define PANELTRACE(...)  // Serial.printf("Display::" __VA_ARGS__)


// ===== static variables (one display only)

/// @brief Active Databus implementation;
extern Arduino_DataBus *bus;

/// @brief Active GFX implementation;
extern Arduino_GFX *gfx;

// static uint16_t DisplayAGFXAdapter_drawcolor;

class DisplayAGFXAdapter : public DisplayAdapter {
public:
  ~DisplayAGFXAdapter() = default;

public:
  virtual bool start() override {
    PANELTRACE("init: w:%d, h:%d, r:%d\n", displayConfig.width, displayConfig.height, displayConfig.rotation);
    PANELTRACE(" colors: #%08x / #%08x / #%08x\n", displayConfig.drawColor, displayConfig.backgroundColor, displayConfig.borderColor);
    PANELTRACE(" invert: %d ips: %d\n", displayConfig.invert, displayConfig.ips);
    PANELTRACE("   pins: light:%d, reset:%d\n", displayConfig.lightPin, displayConfig.resetPin);


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
      gfx->begin(displayConfig.busSpeed);
      gfx->invertDisplay(displayConfig.invert);

      DisplayAdapter::start();

      gfx->setTextWrap(false);
      setColor(displayConfig.drawColor);
      setBackgroundColor(displayConfig.backgroundColor);
      setBorderColor(displayConfig.borderColor);
      _setTextHeight(displayConfig.height > 128 ? 16 : 8);
      clear();
      flush();
    }  // if

    return (true);
  };  // init()


  virtual void setColor(const uint32_t col) override {
    PANELTRACE("setColor #%08x\n", col);
    DisplayAdapter::setColor(col);
    drawColor565 = col565(col);
  };


  virtual void setBackgroundColor(const uint32_t col) override {
    PANELTRACE("setBackgroundColor #%08x\n", col);
    DisplayAdapter::setBackgroundColor(col);
    backColor565 = col565(col);
  };


  virtual void setBorderColor(const uint32_t col) override {
    PANELTRACE("setBorderColor #%08x\n", col);
    DisplayAdapter::setBorderColor(col);
    borderColor565 = col565(col);
  };


  /// @brief Clear all displayed information from the display.
  void clear() override {
    // PANELTRACE("clear #%08x\n", displayConfig.backgroundColor);
    gfx->fillScreen(col565(displayConfig.backgroundColor));
    DisplayAdapter::clear();
  };  // clear()


  /// @brief Draw a text at this position using the specific height.-
  /// @param x x-position or offset of the text.
  /// @param y y-position of the text.
  /// @param h height of the characters box.
  /// @param text the text.
  BoundingBox drawText(int16_t x, int16_t y, int16_t h, const char *text, uint32_t strokeColor) override;


  /// @brief Draw a button
  /// @param x x position of the button
  /// @param y y position of the button
  /// @param w width of the button
  /// @param h height of the button
  /// @param text caption on the button
  virtual void drawButton(int16_t x, int16_t y, int16_t w, int16_t h, const char *text, bool pressed = false) override {
    // LOGGER_JUSTINFO("drawButton: (%d,%d,%d,%d) <%s> %d", x, y, w, h, text, pressed);

    if (displayBox.overlaps(x, y, w, h)) {
      const uint16_t paddingVertical = 4;

      // textbox dimensions
      int16_t bx, by;
      uint16_t bw, bh;

      _setTextHeight(h - (2 * paddingVertical));
      gfx->getTextBounds(text, x, y + baseLine, &bx, &by, &bw, &bh);

      // calculate textbox offset
      int16_t dx = (w - bw) / 2;
      int16_t dy = (h - lineHeight) / 2 - 1;

      uint16_t fCol = (pressed ? backColor565 : drawColor565);
      uint16_t bCol = (pressed ? drawColor565 : backColor565);
      uint16_t r = (h / 2);

      // draw the background
      if (backColor != RGB_UNDEFINED) {
        gfx->fillRoundRect(x, y, w, h, r, bCol);
      }

      // draw the border
      if (borderColor != RGB_UNDEFINED) {
        gfx->drawRoundRect(x, y, w, h, r, borderColor565);
      }

      // draw the text
      gfx->setTextBound(x, y, w, h);
      gfx->setTextColor(fCol);
      gfx->setCursor(x + dx, y + dy + baseLine);
      gfx->print(text);
      _needFlush = true;
    }

  }  // drawButton()


  /// @brief Draw a circle with the given dimensions of the box and an optional inner border.
  void drawCircle(BoundingBox &box, uint32_t borderColor, uint32_t fillColor = RGB_UNDEFINED) override {
    LOGGER_JUSTINFO("drawCircle(%d/%d - %d/%d #%08lx #%08lx)\n", box.x_min, box.y_min, box.x_max, box.y_max, borderColor, fillColor);

    int16_t radius = (box.x_max - box.x_min - 1) / 2;
    int16_t cx = box.x_min + radius;
    int16_t cy = box.y_min + radius;

    if (RGB_IS_COLOR(fillColor)) {
      gfx->fillCircle(cx, cy, radius, col565(fillColor));
    }
    if (RGB_IS_COLOR(borderColor)) {
      gfx->drawCircle(cx, cy, radius, col565(borderColor));
    }

    _needFlush = true;
  };


  // low level pixel drawing

  void startWrite() override {
    gfx->startWrite();
  }

  void writePixel(int16_t x, int16_t y, uint32_t color) override {
    gfx->writePixelPreclipped(x, y, col565(color));
  };

  void endWrite() override {
    DisplayAdapter::endWrite();
    gfx->endWrite();
  }


protected:
  /// @brief return the databus implementation as specified by config.
  Arduino_DataBus *getBus();

  /// @brief send all buffered pixels to display.
  void flush() override {
    gfx->flush();
    DisplayAdapter::flush();
  };  // flush()


  /// @brief convert a 32-bit color value 0x00RRGGBB into the 565 style packed RGB format 0bRRRRRGGGGGGBBBBB.
  /// @param color 24-bit color value
  /// @return 16-bit color in 565 format.
  uint16_t col565(uint32_t color) {

    uint16_t col =
      ((color & 0x00F80000) >> 8)
      | ((color & 0x0000FC00) >> 5)
      | ((color & 0x000000F8) >> 3);
    return (col);
  }


  /// @brief Set height of text in a box.
  /// @param h max. resulting height
  void _setTextHeight(int16_t h) {
    PANELTRACE("setTextHeight(%d)\n", h);
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
      // LOGGER_JUSTINFO(" >(%d,%d)", lineHeight, baseLine);
    }
  }

  // load a builtin font
  void loadFont(int16_t height, int8_t factor = 1) {
    PANELTRACE("loadFont(%d, %d)\n", height, factor);
    const GFXfont *font = nullptr;

    if (height <= 8) {
      // builtin 8pt font
      lineHeight = 8;  // 8+0
      baseLine = 0;
      charWidth = 6;

    } else if (height <= 10) {
      font = &Font_10;
      lineHeight = 10;  // 8 + 2
      baseLine = 7;
      charWidth = 8;

    } else if (height <= 16) {
      font = &Font_16;
      lineHeight = 16;  // 13 + 3
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

  int baseLine;  // baseline offset
  uint16_t backColor565;
  uint16_t borderColor565;
  uint16_t drawColor565;
};
