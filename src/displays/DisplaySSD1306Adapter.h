/**
 * @file DisplaySSD1306Adapter.h
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

#pragma once

#include <displays/DisplayAdapterGFX.h>

#include <Adafruit_SSD1306.h>  // Hardware-specific library for SSD1306
#include <SPI.h>
#include <Wire.h>


class DisplaySSD1306Adapter : public DisplayAdapterGFX {
public:
  ~DisplaySSD1306Adapter() = default;

  bool start() override {
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
