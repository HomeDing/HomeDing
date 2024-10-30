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

#pragma once

#include <displays/DisplayAGFXAdapter.h>

#define TRACE(...)  // Serial.printf("Display::" __VA_ARGS__)


// ===== static variables (one display only)

/// @brief Active Databus implementation;
Arduino_DataBus *bus = nullptr;

/// @brief Active GFX implementation;
Arduino_GFX *gfx = nullptr;


// Helper functions for drawing callbacks in gfxDraw Library

static uint16_t AGFX_drawColor;

void AGFX_drawByColor(int16_t x, int16_t y) {
  gfx->drawPixel(x, y, AGFX_drawColor);
}

// ===== protected functions

Arduino_DataBus *DisplayAGFXAdapter::getBus() {
  TRACE("getbus: %d\n", displayConfig.busmode);
  TRACE("   spi: dc:%d cs:%d clk:%d mosi:%d miso:%d\n",
        displayConfig.dcPin, displayConfig.csPin, displayConfig.spiCLK, displayConfig.spiMOSI, displayConfig.spiMISO);
  TRACE("   i2c: adr:%d, sda:%d, scl:%d\n", displayConfig.i2cAddress, displayConfig.i2cSDA, displayConfig.i2cSCL);

  Arduino_DataBus *bus = nullptr;

  if (displayConfig.busmode == BUSMODE_ANY) {
    if (displayConfig.csPin >= 0) {
      displayConfig.busmode = BUSMODE_SPI;
    } else if (displayConfig.i2cAddress)
      displayConfig.busmode = BUSMODE_I2C;
  }

  if (displayConfig.busmode == BUSMODE_I2C) {
    TRACE("Use I2C\n");
    bus = new Arduino_Wire(displayConfig.i2cAddress, displayConfig.i2cCommandPrefix, displayConfig.i2cDataPrefix);

#if defined(ESP32)
  } else if (displayConfig.busmode == BUSMODE_SPI) {
    TRACE("Use SPI\n");
    bus = new Arduino_HWSPI(displayConfig.dcPin, displayConfig.csPin, displayConfig.spiCLK, displayConfig.spiMOSI, displayConfig.spiMISO);
    // bus = new Arduino_ESP32SPI(displayConfig.dcPin, displayConfig.csPin);

  } else if (displayConfig.busmode == BUSMODE_HSPI) {
    TRACE("Use HSPI\n");
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
    TRACE("Use PAR8\n");

    int pinCount = ListUtils::length(displayConfig.busPins);
    int8_t pins[8];

    if (pinCount != 8) {
      LOGGER_ERR("ST7789 LCD8 bus requires 8 pin definitions");
    } else {
      for (int n = 0; n < 8; n++) {
        pins[n] = Element::_atopin(ListUtils::at(displayConfig.busPins, n).c_str());
      }
    }

    bus = new Arduino_ESP32PAR8Q(
      displayConfig.dcPin, displayConfig.csPin, displayConfig.wrPin, displayConfig.rdPin,
      pins[0], pins[1], pins[2], pins[3], pins[4], pins[5], pins[6], pins[7]);
#endif


#if defined(ESP32) && (CONFIG_IDF_TARGET_ESP32S3) && (ESP_ARDUINO_VERSION_MAJOR < 3)
  } else if (displayConfig.busmode == BUSMODE_LCD8) {
    TRACE("Use LCD8\n");
    bus = new Arduino_ESP32LCD8(
      0 /* DC */,
      GFX_NOT_DEFINED /* CS */,
      47 /* WR */,
      GFX_NOT_DEFINED /* RD */,
      9, 46, 3, 8, 18, 17, 16, 15  //  D0 - D7
    );
#endif


#if defined(ESP8266)
  } else if (displayConfig.busmode == BUSMODE_SPI) {
    TRACE("Use SPI\n");
    // ESP8266 has pre-defined SPI pins
    bus = new Arduino_ESP8266SPI(displayConfig.dcPin, displayConfig.csPin);
#endif
  }  // if
  return (bus);
}  // getBus()



// /// @brief draw a staight line on the display.
// /// @param x0 x coordinate of the starting pixel.
// /// @param y0 y coordinate of the starting pixel.
// /// @param x1 x coordinate of the ending pixel.
// /// @param y1 y coordinate of the ending pixel.
// /// @param color color of the line.
// void DisplayAGFXAdapter::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color) {
//   TRACE("drawLine(%d/%d - %d/%d #%08x)\n", x0, y0, x1, y1, color);

//   if ((color != RGB_UNDEFINED) && (color != RGB_TRANSPARENT) && (x1 >= x0) && (y1 >= y0)) {
//     if (displayBox.overlaps(x0, y0, (x1 - x0 + 1), (y1 - y0 + 1))) {
//       AGFX_drawColor = col565(color);

//       gfxDraw::drawLine(x0, y0, x1, y1, AGFX_drawByColor);
//       _needFlush = true;
//     }
//   }
// }  // drawLine()

// End.
