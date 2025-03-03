/**
 * @file DisplayConfig.h
 * @brief Implementation of the base function for a DisplayXXXElement.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license.
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:
 * * 29.08.2020 created by Matthias Hertel
 * * 17.03.2022 unified HomeDing::DisplayConfig
 */

#pragma once

#define BUSMODE_LIST "x,i2c,spi,hspi,par8lcd8,panel"
#define BUSMODE_ANY 0x00
#define BUSMODE_I2C 0x01
#define BUSMODE_SPI 0x02   // standard SPI interface
#define BUSMODE_HSPI 0x03  // high speed SPI interface on ESP32
#define BUSMODE_PAR8 0x04
#define BUSMODE_LCD8 0x21
#define BUSMODE_PANEL 0x40

namespace HomeDing {

class DisplayConfig {
public:
  DisplayConfig();

  /// @brief Width of the display
  uint16_t width;

  /// @brief Height of the display
  uint16_t height;

  /// @brief Brightness of display 0...100 (percent)
  uint16_t brightness;

  /// @brief Default Stroke Color
  uint32_t drawColor;

  /// @brief Default Fill Color
  uint32_t backgroundColor;

  /// @brief Rotation of the display.
  int rotation;

  /// @brief Row-Offset in the display memory for the displayed part.
  int rowOffset;

  /// @brief Column-Offset in the display memory for the displayed part.
  int colOffset;

  /// @brief Pin to reset the display chip.
  int resetPin;

  /// @brief Pin to enable backlight.
  int lightPin;

  /// @brief Pin to invert the display colors.
  bool invert;

  /// @brief TFT IPS panel in use.
  bool ips;

  /// @brief Bus mode.
  int busmode;

  /// @brief Size of default font used at startup.
  int fontsize;

  // bus configurations for any bus

  int32_t busSpeed = -1;  // speed
  int csPin = -1;         // chip select pin (SPI, lcd8, bus16)
  int dcPin = -1;         // Data-Command pin (SPI, lcd8, bus16)
  int wrPin = -1;         // write strobe pin (lcd8)
  int rdPin = -1;         // read strobe pin (lcd8)

  /* ===== I2C interface ===== */

  int i2cAddress;  ///< i2c address
  int i2cSDA;      ///< i2c data pin
  int i2cSCL;      ///< i2c clock pin

  // prefix bytes for I2c bus
  uint8_t i2cCommandPrefix;
  uint8_t i2cDataPrefix;

  /* ===== SPI interface ===== */
  int spiMOSI;  ///< SPI interface MOSI pin
  int spiMISO;  ///< SPI interface MISO pin
  int spiCLK;   ///< SPI interface clock CLK pin

  /* ===== RGB Parallel / Panel ===== */

  int dePin;

  int hsync_pin;
  uint16_t hsync_polarity;
  uint16_t hsync_pulse_width;
  uint16_t hsync_front_porch;
  uint16_t hsync_back_porch;

  int vsync_pin;
  uint16_t vsync_polarity;
  uint16_t vsync_pulse_width;
  uint16_t vsync_front_porch;
  uint16_t vsync_back_porch;

  int pclk_pin;

  // GPIO pins used for the 8-bit and 16-bit busses
  String busPins;
};

// global, singleton display configuration
extern DisplayConfig displayConfig;  // allocated in  DisplayConfig.cpp

}

// End
