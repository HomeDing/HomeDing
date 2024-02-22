/**
 * @file DisplayElement.h
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
 * * 17.03.2022 unified DisplayConfig
 */

#pragma once

#include <functional>

#include <displays/BoundingBox.h>

class DisplayAdapter;

#define RGB_WHITE 0x00FFFFFF
#define RGB_BLACK 0x00000000
#define RGB_GRAY 0x00808080
#define RGB_SILVER 0x00C0C0C0
#define RGB_RED 0x00FF0000
#define RGB_GREEN 0x0000FF00
#define RGB_BLUE 0x000000FF
#define RGB_YELLOW 0x00FFFF00
#define RGB_ORANGE 0x00FFA500
#define RGB_PURPLE 0x00800080

#define RGB_UNDEFINED 0xEE000000  // key color
#define RGB_TRANSPARENT 0xEF000000  // key color

#define RGB_IS_COLOR(col) ((col != RGB_UNDEFINED) && (col != RGB_TRANSPARENT))
#define RGB_NO_COLOR(col) ((col == RGB_UNDEFINED) || (col == RGB_TRANSPARENT))


#define BUSMODE_ANY   0x00
#define BUSMODE_I2C   0x01
#define BUSMODE_SPI   0x02  // standard SPI interface
#define BUSMODE_HSPI  0x03  // high speed SPI interface on ESP32
#define BUSMODE_PAR8  0x04
#define BUSMODE_LCD8  0x21
#define BUSMODE_PANEL 0x40


struct DisplayConfig {
  /** Width of display */
  uint16_t width = 128;

  /** Height of display */
  uint16_t height = 64;

  /// @brief Brightness of display 0...100 (percent)
  uint16_t brightness = 50;

  /** Default Draw & Background Color */
  uint32_t drawColor = RGB_WHITE;
  uint32_t backgroundColor = RGB_BLACK;
  uint32_t borderColor = RGB_WHITE;

  /** Rotation of the display */
  int rotation = 0;

  /// @brief Row-Offset in the display memory for the displayed part.
  int rowOffset = 0;

  /// @brief Column-Offset in the display memory for the displayed part.
  int colOffset = 0;

  /** Pin to reset the display chip */
  int resetPin = -1;

  /** Pin to enable backlight etc. */
  int lightPin = -1;

  /** Pin to invert the display colors */
  bool invert = false;

  /// @brief TFT IPS panel in use.
  bool ips = false;

  int busmode = BUSMODE_ANY;

  // bus configurations for any bus

  int32_t busSpeed = -1;  // speed
  int csPin = -1;         // chip select pin (SPI, lcd8, bus16)
  int dcPin = -1;         // Data-Command pin (SPI, lcd8, bus16)
  int wrPin = -1;         // write strobe pin (lcd8)
  int rdPin = -1;         // read strobe pin (lcd8)

  // GPIO pins used for the 8-bit and 16-bit busses
  String busPins;

  /* ===== I2C interface ===== */

  int i2cAddress = 0;  ///< i2c address
  int i2cSDA = 0;      ///< i2c data pin
  int i2cSCL = 0;      ///< i2c clock pin

  // prefix bytes for I2c bus
  uint8_t i2cCommandPrefix;
  uint8_t i2cDataPrefix;

  /* ===== SPI interface ===== */
  int spiMOSI = -1;  ///< SPI interface MOSI pin
  int spiMISO = -1;  ///< SPI interface MISO pin
  int spiCLK = -1;   ///< SPI interface clock CLK pin
};


class DisplayElement : public Element {
public:
  /* ===== Element functions ===== */

  /**
   * @brief Constructor of a new DisplayElement.
   */
  DisplayElement();

  /**
   * @brief initialize the DisplayElement.
   * @param board The board reference.
   */
  virtual void init(Board *board);

  /**
   * @brief Set a parameter or property to a new value or start an action.
   * @param name Name of property.
   * @param value Value of property.
   * @return true when property could be changed and the corresponding action
   * could be executed.
   */
  virtual bool set(const char *name, const char *value) override;

  /// @brief Activate the Element.
  virtual void start() override;

  /// @brief Activate the Element using the given adapter.
  virtual void start(DisplayAdapter *displayAdapter);

  /**
   * @brief push the current value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void pushState(
    std::function<void(const char *pName, const char *eValue)> callback) override;

  // === common properties for initializing display adapters

protected:
  struct DisplayConfig config;

  /** event when page changes */
  String _onPage;

private:
  void _newPage(int page);
};

// This is a base class that will not be registered
