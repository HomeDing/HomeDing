/**
 * @file DisplayST7735Adapter.h
 *
 * @brief DisplayAdapter implementation for the HomeDing library
 * adapting LCD displays using the ST7735 chip.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * -----
 * * 21.10.2022 created by Matthias Hertel
 */

#pragma once

#include <displays/DisplayAdapterGFX.h>

#include <Adafruit_ST7735.h>  // Hardware-specific library for ST7735
#include <SPI.h>

class DisplayST7735Adapter : public DisplayAdapterGFX {
public:
  ~DisplayST7735Adapter() = default;

  bool start() override {
    // LOGGER_JUSTINFO("init: w:%d, h:%d, r:%d", conf->width, conf->height, conf->rotation);
    // LOGGER_JUSTINFO("  pins: l:%d, r:%d", conf->lightPin, conf->resetPin);
    // LOGGER_JUSTINFO("   i2c: adr:%d, sda:%d, scl:%d", conf->i2cAddress, conf->i2cSDA, conf->i2cSCL);
    // LOGGER_JUSTINFO("   spi: cs:%d, dc:%d, mosi:%d, miso:%d, clk:%d", conf->spiCS, conf->spiDC, conf->spiMOSI, conf->spiMISO, conf->spiCLK);

#if defined(ESP8266)
    display = new (std::nothrow) Adafruit_ST7735(conf->spiCS, conf->spiDC, conf->resetPin);

#elif defined(ESP32)
    SPI.begin(conf->spiCLK, conf->spiMISO, conf->spiMOSI);
    // display = new (std::nothrow) Adafruit_ST7735(conf->spiCS, conf->spiDC, conf->spiMOSI, conf->spiCLK, conf->resetPin);
    display = new (std::nothrow) Adafruit_ST7735(&SPI, conf->spiCS, conf->spiDC, conf->resetPin);
#endif

    if (!display) {
      LOGGER_ERR("not found");
      return (false);

    } else {
      gfxDisplay = (Adafruit_GFX *)display;
      // display->init(conf->width, conf->height, SPI_MODE0);
      display->initR(INITR_MINI160x80);
      display->setSPISpeed(40000000);
      display->invertDisplay(conf->invert);
      DisplayAdapterGFX::start();
      display->invertDisplay(conf->invert);
    }  // if
    return (true);
  };   // init()


  virtual void setColor(const uint32_t col) override {
    // swap RGB to BGR
    uint32_t col2 = ((col & 0x00FF0000) >> 16) | (col & 0x0000FF00) | ((col & 0x000000FF) << 16);
    DisplayAdapterGFX::setColor(col2);
  };

private:
  /**
   * @brief Reference to the used library object
   */
  Adafruit_ST7735 *display = nullptr;
};
