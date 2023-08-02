/**
 * @file DisplayST7789Adapter.h
 *
 * @brief DisplayAdapter implementation for the HomeDing library
 * adapting TFT displays using the ST7789 chip.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * -----
 * * 25.07.2018 created by Matthias Hertel
 * * 07.12.2020 no write text beyond textline end.
 */

#pragma once

#include <displays/DisplayAdapterGFX.h>

#include <Adafruit_ST7789.h>  // Hardware-specific library for ST7789
#include <SPI.h>

class DisplayST7789Adapter : public DisplayAdapterGFX {
public:
  ~DisplayST7789Adapter() = default;

  bool start() override {
    // LOGGER_JUSTINFO("init: w:%d, h:%d, r:%d", conf->width, conf->height, conf->rotation);
    // LOGGER_JUSTINFO("  pins: l:%d, r:%d", conf->lightPin, conf->resetPin);
    // LOGGER_JUSTINFO("   i2c: adr:%d, sda:%d, scl:%d", conf->i2cAddress, conf->i2cSDA, conf->i2cSCL);
    // LOGGER_JUSTINFO("   spi: cs:%d, dc:%d, mosi:%d, miso:%d, clk:%d", conf->spiCS, conf->spiDC, conf->spiMOSI, conf->spiMISO, conf->spiCLK);

#if defined(ESP8266)
    display = new (std::nothrow) Adafruit_ST7789(conf->spiCS, conf->spiDC, conf->resetPin);

#elif defined(ESP32)
    SPI.begin(conf->spiCLK, conf->spiMISO, conf->spiMOSI);
    // display = new (std::nothrow) Adafruit_ST7789(conf->spiCS, conf->spiDC, conf->spiMOSI, conf->spiCLK, conf->resetPin);
    display = new (std::nothrow) Adafruit_ST7789(&SPI, conf->spiCS, conf->spiDC, conf->resetPin);
#endif

    if (!display) {
      LOGGER_ERR("not found");
      return (false);

    } else {
      gfxDisplay = (Adafruit_GFX *)display;
      display->init(conf->width, conf->height, SPI_MODE0);
      display->setSPISpeed(40000000);
      display->invertDisplay(conf->invert);
      DisplayAdapterGFX::start();
    }  // if
    return (true);
  };  // init()

private:
  /**
   * @brief Reference to the used library object
   */
  Adafruit_ST7789 *display = nullptr;
};
