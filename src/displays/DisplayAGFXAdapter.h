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

#include <displays/DisplayAdapter.h>

using namespace HomeDing;

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
  /// @brief initialize and start the display
  /// @return display is working
  virtual bool start() override;

  /// @brief Clear all displayed information from the display.
  void clear() override {
    // PANELTRACE("clear #%08x\n", displayConfig.backgroundColor);
    gfx->fillScreen(col565(displayConfig.backgroundColor));
    DisplayAdapter::clear();
  };  // clear()


  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color) override {
    // PANELTRACE("fillRect %d/%d %d/%d #%08x\n", x, y, w, h, color);
    DisplayAdapter::fillRect(x, y, w, h, color);
    gfx->fillRect(x, y, w, h, col565(color));
  };


  /// @brief calculate the Bounding box of a text drawn at 0/0
  /// @param h font height
  /// @param text the text.
  /// @return Dimension of the text with box.x and .y == 0.
  BoundingBox textBox(int16_t h, const char *text) override;


  /// @brief Draw a text at this position using the specific height.-
  /// @param x x-position or offset of the text.
  /// @param y y-position of the text.
  /// @param h height of the characters box.
  /// @param text the text.
  BoundingBox drawText(int16_t x, int16_t y, int16_t h, const char *text, uint32_t strokeColor) override;


  // ===== low level pixel drawing

  void startWrite() override {
    DisplayAdapter::startWrite();
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
  /// @brief send all buffered pixels to display.
  void flush() override;

  /// @brief return the databus implementation as specified by config.
  Arduino_DataBus *getBus();

  /// @brief convert a 32-bit color value 0x00RRGGBB into the 565 style packed RGB format 0bRRRRRGGGGGGBBBBB.
  /// @param color 24-bit color value
  /// @return 16-bit color in 565 format.
  uint16_t col565(uint32_t color);


  /// @brief Set height of text in a box.
  /// @param h max. resulting height
  void _setTextHeight(int16_t h);


  // load a builtin font
  void loadFont(int16_t height, int8_t factor = 1);

  int baseLine;  // baseline offset
};
