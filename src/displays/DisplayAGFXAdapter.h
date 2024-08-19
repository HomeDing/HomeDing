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

#define UNUSED(expr) do { (void)(expr); } while (0)

#include <Arduino_GFX_Library.h>

#include <displays/DisplayAdapter.h>

#if defined(ESP8266)
#include <pgmspace.h>
#undef PROGMEM
#define PROGMEM STORE_ATTR

#elif !defined(PROGMEM)
#define PROGMEM

#endif

#include <fonts/font.h>

#define PANELTRACE(...)  // Serial.printf("Display::" __VA_ARGS__)

class DisplayAGFXAdapter : public DisplayAdapter {
public:
  ~DisplayAGFXAdapter() = default;

  Arduino_DataBus *getBus(int busmode, DisplayConfig *conf) {
    PANELTRACE("getbus: %d\n", busmode);
    PANELTRACE("   spi: dc:%d cs:%d clk:%d mosi:%d miso:%d\n",
               conf->dcPin, conf->csPin, conf->spiCLK, conf->spiMOSI, conf->spiMISO);
    PANELTRACE("   i2c: adr:%d, sda:%d, scl:%d\n", conf->i2cAddress, conf->i2cSDA, conf->i2cSCL);

    Arduino_DataBus *bus = nullptr;

    if (busmode == BUSMODE_ANY) {
      if (conf->csPin >= 0) {
        busmode = BUSMODE_SPI;
      } else if (conf->i2cAddress)
        busmode = BUSMODE_I2C;
    }

    if (busmode == BUSMODE_I2C) {
      PANELTRACE("Use I2C\n");
      bus = new Arduino_Wire(conf->i2cAddress, conf->i2cCommandPrefix, conf->i2cDataPrefix);

#if defined(ESP32)
    } else if (busmode == BUSMODE_SPI) {
      PANELTRACE("Use SPI\n");
      bus = new Arduino_HWSPI(conf->dcPin, conf->csPin, conf->spiCLK, conf->spiMOSI, conf->spiMISO);
      // bus = new Arduino_ESP32SPI(conf->dcPin, conf->csPin);

    } else if (busmode == BUSMODE_HSPI) {
      PANELTRACE("Use HSPI\n");
      bus = new Arduino_ESP32SPI(
        conf->dcPin,
        conf->csPin,
        conf->spiCLK,
        conf->spiMOSI,
        conf->spiMISO,
        HSPI /* spi_num */
      );
#endif

#if defined(ESP32) && (CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3)
    } else if (busmode == BUSMODE_PAR8) {
      PANELTRACE("Use PAR8\n");

      int pinCount = ListUtils::length(conf->busPins);
      int8_t pins[8];

      if (pinCount != 8) {
        LOGGER_ERR("ST7789 LCD8 bus requires 8 pin definitions");
      } else {
        for (int n = 0; n < 8; n++) {
          pins[n] = Element::_atopin(ListUtils::at(conf->busPins, n).c_str());
        }
      }

      bus = new Arduino_ESP32PAR8Q(
        conf->dcPin, conf->csPin, conf->wrPin, conf->rdPin,
        pins[0], pins[1], pins[2], pins[3], pins[4], pins[5], pins[6], pins[7]);
#endif


#if defined(ESP32) && (CONFIG_IDF_TARGET_ESP32S3) && (ESP_ARDUINO_VERSION_MAJOR < 3)
    } else if (busmode == BUSMODE_LCD8) {
      PANELTRACE("Use LCD8\n");
      bus = new Arduino_ESP32LCD8(
        0 /* DC */,
        GFX_NOT_DEFINED /* CS */,
        47 /* WR */,
        GFX_NOT_DEFINED /* RD */,
        9, 46, 3, 8, 18, 17, 16, 15  //  D0 - D7
      );
#endif


#if defined(ESP8266)
    } else if (busmode == BUSMODE_SPI) {
      PANELTRACE("Use SPI\n");
      // ESP8266 has pre-defined SPI pins
      bus = new Arduino_ESP8266SPI(conf->dcPin, conf->csPin);
#endif
    }  // if
    return (bus);
  };


  Arduino_DataBus *getBus(DisplayConfig *conf) {
    return (getBus(conf->busmode, conf));
  };

  virtual bool start() override {
    PANELTRACE("init: w:%d, h:%d, r:%d\n", conf->width, conf->height, conf->rotation);
    PANELTRACE(" colors: #%08x / #%08x / #%08x\n", conf->drawColor, conf->backgroundColor, conf->borderColor);
    PANELTRACE(" invert: %d ips: %d\n", conf->invert, conf->ips);
    PANELTRACE("   pins: light:%d, reset:%d\n", conf->lightPin, conf->resetPin);


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
      gfx->begin(conf->busSpeed);
      gfx->invertDisplay(conf->invert);

      DisplayAdapter::start();

      gfx->setTextWrap(false);
      setColor(conf->drawColor);
      setBackgroundColor(conf->backgroundColor);
      setBorderColor(conf->borderColor);
      _setTextHeight(conf->height > 128 ? 16 : 8);
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
    // PANELTRACE("clear #%08x\n", conf->backgroundColor);
    gfx->fillScreen(col565(conf->backgroundColor));
    DisplayAdapter::clear();
  };  // clear()


  /**
   * @brief Draw a text at this position using the specific height.-
   * @param x x-position or offset of the text.
   * @param y y-position of the text.
   * @param size height of the characters box.
   * @param text the text.
   */
  int drawText(int16_t x, int16_t y, int16_t h, const char *text) override {
    PANELTRACE("drawText: %d/%d h:%d t:<%s>\n", x, y, h, text);
    PANELTRACE("  colors: %04x on %04x\n", drawColor565, backColor565);

    if (displayBox.contains(x, y)) {
      // textbox dimensions
      int16_t bx, by;
      uint16_t bw, bh;

      gfx->setTextBound(0, 0, gfx->width(), gfx->height());

      _setTextHeight(h);
      gfx->getTextBounds(text, x, y + baseLine, &bx, &by, &bw, &bh);
      PANELTRACE("     box: %d/%d w:%d h:%d\n", bx, by, bw, bh);

      gfx->setTextColor(drawColor565, drawColor565);  // transparent background
      gfx->setCursor(x, y + baseLine);
      gfx->print(text);
      _needFlush = true;
      return ((bx - x) + bw);

    } else {
      return (0);
    }

  }  // drawText


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

  virtual void drawPixel(int16_t x, int16_t y, uint32_t color) override {
    if (RGB_IS_COLOR(color)) {
      gfx->drawPixel(x, y, col565(color));
      _needFlush = true;
    }
  };

  virtual void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color) override {
    PANELTRACE("drawLine(%d/%d - %d/%d #%08x)\n", x0, y0, x1, y1, color);

    if ((color != RGB_UNDEFINED) && (color != RGB_TRANSPARENT) && (x1 >= x0) && (y1 >= y0)) {
      if (displayBox.overlaps(x0, y0, (x1 - x0 + 1), (y1 - y0 + 1))) {
        gfx->drawLine(x0, y0, x1, y1, col565(color));
        _needFlush = true;
      }
    }
  }  // drawLine()


  /// @brief Draw a rectangle with the given dimensions of the box and an optional inner border.
  virtual void drawRectangle(BoundingBox &box, uint32_t borderColor, uint32_t fillColor = RGB_UNDEFINED) override {
    PANELTRACE("drawRectangle(%d/%d - %d/%d #%08x #%08x)\n", box.x_min, box.y_min, box.x_max, box.y_max, borderColor, fillColor);

    if ((!box.isEmpty()) && (displayBox.overlaps(box))) {
      int16_t w = box.x_max - box.x_min + 1;
      int16_t h = box.y_max - box.y_min + 1;
      bool bFill = RGB_IS_COLOR(fillColor);
      uint16_t fill = col565(fillColor);

      if (RGB_NO_COLOR(borderColor)) {
        // draw without border
        if (bFill) {
          gfx->fillRect(box.x_min, box.y_min, w, h, fill);
          _needFlush = true;
        }

      } else {
        if (bFill) {
          gfx->fillRect(box.x_min + 1, box.y_min + 1, box.x_max - box.x_min - 1, box.y_max - box.y_min - 1, fill);
        }
        gfx->drawRect(box.x_min, box.y_min, w, h, col565(borderColor));
        _needFlush = true;
      }
    }
  }  // drawRectangle()


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


protected:
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

  /**
   * @brief Reference to the used library object
   */
  Arduino_DataBus *bus = nullptr;
  Arduino_GFX *gfx = nullptr;


  int baseLine;  // baseline offset
  uint16_t backColor565;
  uint16_t borderColor565;
  uint16_t drawColor565;
};
