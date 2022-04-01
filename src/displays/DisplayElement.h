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

#ifndef DISPLAYELEMENT_H
#define DISPLAYELEMENT_H

#include <functional>

struct DisplayConfig {
  /** Width of display */
  int width = 128;

  /** Height of display */
  int height = 64;

  /** Brightness of display 0...100 (percent) */
  int brightness = 80;

  /** Rotation of the display */
  int rotation = 0;

  /** Pin to reset the display chip */
  int resetPin = -1;

  /** Pin to enable backlight etc. */
  int lightPin = -1;

  /** Pin to reset the display chip */
  bool invert = -false;

  /* ===== I2C interface ===== */

  int i2cAddress = 0;  ///< i2c address
  int i2cSDA = 0;      ///< i2c data
  int i2cSCL = 0;      ///< i2c clock

  /* ===== SPI interface ===== */
  int spiMOSI = -1;  ///< SPI interface MOSI pin
  int spiMISO = -1;  ///< SPI interface MISO pin
  int spiCLK = -1;   ///< SPI interface clock CLK pin
  int spiCS = -1;    ///< SPI interface chip select CS pin
  int spiDC = -1;    ///< SPI interface Data-Command DC pin
  int spiRST = -1;
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

  /**
   * @brief Activate the Element.
   */
  virtual void start() override;

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
  void _reset();

  void _newPage(int page);
};

// This is a base class that will not be registered

#endif  // DISPLAYELEMENT_H