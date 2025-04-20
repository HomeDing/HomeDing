/**
 * @file DisplayAGFXAdapter.cpp
 *
 * @brief DisplayAdapter implementation for the HomeDing library
 * for displays using the Arduino_GFX library.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * -----
 */

#include <displays/DisplayAGFXAdapter.h>

#define TRACE(...)  // LOGGER_TRACE(__VA_ARGS__)

// ===== static variables (one display only)

/// @brief Active Databus implementation;
Arduino_DataBus *bus = nullptr;

/// @brief Active GFX implementation;
Arduino_GFX *gfx = nullptr;


// Helper functions for drawing callbacks in gfxDraw Library

static uint16_t AGFX_drawColor;

bool DisplayAGFXAdapter::start() {
  PANELTRACE("init: w:%d, h:%d, r:%d\n", displayConfig.width, displayConfig.height, displayConfig.rotation);
  PANELTRACE(" colors: #%08x / #%08x\n", displayConfig.drawColor, displayConfig.backgroundColor);
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
    PANELTRACE("gfx found.\n");
    gfx->begin(displayConfig.busSpeed);
    gfx->invertDisplay(displayConfig.invert);

    DisplayAdapter::start();

    gfx->setTextWrap(false);
    _setTextHeight(displayConfig.fontsize);

    clear();
    flush();
  }  // if

  return (true);
};  // start()


/// return the Bounding box of a text drawn at 0/0

BoundingBox DisplayAGFXAdapter::textBox(int16_t h, const char *text) {
  TRACE("textBox: h:%d t:\"%s\"", h, text);

  int16_t bx, by;
  uint16_t bw, bh;

  gfx->setTextBound(0, 0, gfx->width(), gfx->height());

  _setTextHeight(h);  // also sets baseLine
  gfx->getTextBounds(text, 0, 0 + baseLine, &bx, &by, &bw, &bh);
  TRACE("  bounds: %d/%d w:%d h:%d", bx, by, bw, bh);
  // bx and by might be > 0 because of the first character is starting with some space.
  // therefore by, by is added to the width/height.
  BoundingBox box(0, 0, bx + bw - 1, by + bh - 1);
  TRACE("     box: %d -- %d / %d -- %d", box.x_min, box.y_min, box.x_max, box.y_max);
  return (box);
};


BoundingBox DisplayAGFXAdapter::drawText(int16_t x, int16_t y, int16_t h, const char *text, uint32_t strokeColor) {
  TRACE("drawText: %d/%d h:%d t:\"%s\" color:%08lx", x, y, h, text, strokeColor);

  if (displayBox.contains(x, y)) {
    // textbox dimensions
    uint16_t col = col565(strokeColor);

    // done in textBox()...
    // gfx->setTextBound(0, 0, gfx->width(), gfx->height());
    // _setTextHeight(h);

    BoundingBox b = textBox(h, text);
    TRACE(" boundbox: %d/%d -- %d/%d", b.x_min, b.x_max, b.y_min, b.y_max);

    gfx->setTextColor(col, col);  // transparent background
    gfx->setCursor(x, y + baseLine);
    gfx->print(text);
    _needFlush = true;

    b.shift(x, y);
    return (b);

  } else {
    BoundingBox b;
    TRACE("    nobox: %d -- %d / %d -- %d", b.x_min, b.x_max, b.y_min, b.y_max);
    return (b);
  }

}  // drawText


// ===== protected functions

/// @brief send all buffered pixels to display.
void DisplayAGFXAdapter::flush() {
  PANELTRACE("flush()\n");
  gfx->flush();
  DisplayAdapter::flush();
};  // flush()


Arduino_DataBus *DisplayAGFXAdapter::getBus() {
  TRACE("getBus: %d", displayConfig.busmode);
  TRACE("   spi: dc:%d cs:%d clk:%d mosi:%d miso:%d",
        displayConfig.dcPin, displayConfig.csPin, displayConfig.spiCLK, displayConfig.spiMOSI, displayConfig.spiMISO);
  TRACE("   i2c: adr:%d, sda:%d, scl:%d", displayConfig.i2cAddress, displayConfig.i2cSDA, displayConfig.i2cSCL);

  Arduino_DataBus *bus = nullptr;

  if (displayConfig.busmode == BUSMODE_ANY) {
    if (displayConfig.csPin >= 0) {
      displayConfig.busmode = BUSMODE_SPI;
    } else if (displayConfig.i2cAddress)
      displayConfig.busmode = BUSMODE_I2C;
  }

  if (displayConfig.busmode == BUSMODE_I2C) {
    TRACE("Use I2C");
    bus = new Arduino_Wire(displayConfig.i2cAddress, displayConfig.i2cCommandPrefix, displayConfig.i2cDataPrefix);

#if defined(ESP32)
  } else if (displayConfig.busmode == BUSMODE_SPI) {
    TRACE("Use SPI");
    bus = new Arduino_HWSPI(displayConfig.dcPin, displayConfig.csPin, displayConfig.spiCLK, displayConfig.spiMOSI, displayConfig.spiMISO);
    // bus = new Arduino_ESP32SPI(displayConfig.dcPin, displayConfig.csPin);

  } else if (displayConfig.busmode == BUSMODE_HSPI) {
    TRACE("Use HSPI");
    bus = new Arduino_ESP32SPI(
      displayConfig.dcPin,
      displayConfig.csPin,
      displayConfig.spiCLK,
      displayConfig.spiMOSI,
      displayConfig.spiMISO,
      HSPI /* spi_num */
    );
#endif

#if defined(ESP32) && (CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3)
  } else if (displayConfig.busmode == BUSMODE_PAR8) {
    TRACE("Use PAR8");

    int pinCount = ListUtils::length(displayConfig.busPins);
    int8_t pins[8];

    if (pinCount != 8) {
      LOGGER_ERR("PAR8 bus requires 8 pin definitions");
    } else {
      for (int n = 0; n < 8; n++) {
        pins[n] = Element::_atopin(ListUtils::at(displayConfig.busPins, n).c_str());
      }
    }

    bus = new Arduino_ESP32PAR8Q(
      displayConfig.dcPin, displayConfig.csPin, displayConfig.wrPin, displayConfig.rdPin,
      pins[0], pins[1], pins[2], pins[3], pins[4], pins[5], pins[6], pins[7]);
#endif


#if defined(ESP32) && (CONFIG_IDF_TARGET_ESP32S3) // && (ESP_ARDUINO_VERSION_MAJOR < 3)
  } else if (displayConfig.busmode == BUSMODE_LCD8) {
    TRACE("Use LCD8");

    int pinCount = ListUtils::length(displayConfig.busPins);
    int8_t pins[8];

    if (pinCount != 8) {
      LOGGER_ERR("LCD8 bus requires 8 pin definitions");
    } else {
      for (int n = 0; n < 8; n++) {
        pins[n] = Element::_atopin(ListUtils::at(displayConfig.busPins, n).c_str());
      }
    }

    bus = new Arduino_ESP32LCD8(
      displayConfig.dcPin, displayConfig.csPin, displayConfig.wrPin, displayConfig.rdPin,
      pins[0], pins[1], pins[2], pins[3], pins[4], pins[5], pins[6], pins[7]);
#endif


#if defined(ESP8266)
  } else if (displayConfig.busmode == BUSMODE_SPI) {
    TRACE("Use SPI");
    // ESP8266 has pre-defined SPI pins
    bus = new Arduino_ESP8266SPI(displayConfig.dcPin, displayConfig.csPin);
#endif
  }  // if
  return (bus);
}  // getBus()


/// @brief convert a 32-bit color value 0x00RRGGBB into the 565 style packed RGB format 0bRRRRRGGGGGGBBBBB.
/// @param color 24-bit color value
/// @return 16-bit color in 565 format.
uint16_t DisplayAGFXAdapter::col565(uint32_t color) {

  uint16_t col =
    ((color & 0x00F80000) >> 8)
    | ((color & 0x0000FC00) >> 5)
    | ((color & 0x000000F8) >> 3);
  return (col);
}  // col565()


/// @brief Set height of text in a box.
/// @param h max. resulting height
void DisplayAGFXAdapter::_setTextHeight(int16_t h) {
  PANELTRACE("_setTextHeight(%d)\n", h);
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
  }
}  // _setTextHeight()


// load a builtin font
void DisplayAGFXAdapter::loadFont(int16_t height, int8_t factor) {
  PANELTRACE("loadFont(%d, %d)\n", height, factor);
  const GFXfont *font = nullptr;

  if (height <= 8) {
    // builtin 8pt font
    baseLine = 0;

  } else if (height <= 10) {
    font = &Font_10;
    baseLine = 7;

  } else if (height <= 16) {
    font = &Font_16;
    baseLine = 12;

  } else if (height <= 24) {
    font = &Font_24;
    baseLine = 19;
  }  // if

  gfx->setFont(font);
  gfx->setTextSize(factor);
  baseLine *= factor;
}  // loadFont()



// End.
