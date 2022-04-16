/**
 * @file DisplayAdapterSSD1306.h
 *
 * @brief DisplayAdapter implementation for the HomeDing library
 * adapting OLED displays using the SSD1306 chip.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * -----
 * * 25.07.2018 created by Matthias Hertel
 * * 18.03.2022 based on Adafruit GFX driver
 */

#ifndef DisplayAdapterSSD1306_H
#define DisplayAdapterSSD1306_H

#include <displays/DisplayAdapterGFX.h>

#include <Adafruit_SSD1306.h>  // Hardware-specific library for SSD1306
#include <SPI.h>
#include <Wire.h>


class DisplayAdapterSSD1306 : public DisplayAdapterGFX {
public:
  ~DisplayAdapterSSD1306() = default;

  bool start() override {
    // LOGGER_JUSTINFO("init: w:%d, h:%d, r:%d", conf->width, conf->height, conf->rotation);
    // LOGGER_JUSTINFO("  pins: l:%d, r:%d", conf->lightPin, conf->resetPin);
    // LOGGER_JUSTINFO("   i2c: adr:%d, sda:%d, scl:%d", conf->i2cAddress, conf->i2cSDA, conf->i2cSCL);
    // LOGGER_JUSTINFO("   spi: cs:%d, dc:%d, mosi:%d, clk:%d", conf->spiCS, conf->spiDC, conf->spiMOSI, conf->spiCLK);

    display = new (std::nothrow) Adafruit_SSD1306(conf->width, conf->height, &Wire, conf->resetPin);

    if (!display) {
      LOGGER_ERR("not found");
      return (false);

    } else {
      gfxDisplay = (Adafruit_GFX *)display;
      display->begin(SSD1306_SWITCHCAPVCC, conf->i2cAddress);
      backColor565 = SSD1306_BLACK;
      drawColor565 = SSD1306_WHITE;
      DisplayAdapterGFX::start();
    }  // if
    return (true);
  };  // init()


  virtual void setBrightness(uint8_t bright) override {
    display->dim(bright < 50);
  };

  virtual void setColor(uint32_t UNUSED col) override {
    // LOGGER_JUSTINFO("no-setColor");
  };

  virtual uint32_t getColor() override {
    return(0x00FFFFFF); // white
  };

  /**
   * @brief The flush method must be called after every output sequence to allow
   * combined sending new information to the display.
   */
  virtual void flush() override{
    // LOGGER_JUSTINFO("flush()");
    display->display();
  };


private:
  /**
   * @brief Reference to the used library object
   */
  Adafruit_SSD1306 *display = nullptr;
};

#endif  // DisplayAdapterSSD1306_H