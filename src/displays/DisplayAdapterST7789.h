/**
 * @file DisplayAdapterST7789.h
 *
 * @brief DisplayAdapter implementation for the HomeDing library
 * adapting LCD displays using the HD44780 chip.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * -----
 * * 25.07.2018 created by Matthias Hertel
 * * 07.12.2020 no write text beyond textline end.
 */

#ifndef DisplayAdapterST7789_H
#define DisplayAdapterST7789_H

#include <displays/DisplayAdapterGFX.h>

#include <Adafruit_ST7789.h>  // Hardware-specific library for ST7789
#include <SPI.h>

class DisplayAdapterST7789 : public DisplayAdapterGFX {
public:

  ~DisplayAdapterST7789() = default;

  bool start() override {
    // LOGGER_JUSTINFO("init: w:%d, h:%d, r:%d", conf->width, conf->height, conf->rotation);
    // LOGGER_JUSTINFO("  pins: l:%d, r:%d", conf->lightPin, conf->resetPin);
    // LOGGER_JUSTINFO("   i2c: adr:%d, sda:%d, scl:%d", conf->i2cAddress, conf->i2cSDA, conf->i2cSCL);
    // LOGGER_JUSTINFO("   spi: cs:%d, dc:%d, mosi:%d, clk:%d", conf->spiCS, conf->spiDC, conf->spiMOSI, conf->spiCLK);

    display = new (std::nothrow) Adafruit_ST7789(conf->spiCS, conf->spiDC, conf->spiMOSI, conf->spiCLK, conf->resetPin);

    if (!display) {
      LOGGER_ERR("not found");
      return (false);

    } else {
      gfxDisplay = (Adafruit_GFX *)display;
      display->init(conf->width, conf->height, SPI_MODE0);
      display->setSPISpeed(40000000);
      display->invertDisplay(conf->invert);
      backColor565 = ST77XX_BLACK;
      drawColor565 = ST77XX_WHITE;
      DisplayAdapterGFX::start();
    }  // if
    return (true);
  };  // init()


  // virtual void setBrightness(uint8_t bright) override {
  //   display->drawBitmap((bright * 128) / 100);
  // };

  // no flush() required.

private:
  /**
   * @brief Reference to the used library object
   */
  Adafruit_ST7789 *display = nullptr;
};

#endif  // DisplayAdapterST7789_H