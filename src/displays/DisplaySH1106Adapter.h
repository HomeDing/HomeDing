/**
 * @file DisplaySH1106Adapter.h
 *
 * @brief DisplayAdapter implementation for the HomeDing library
 * adapting OLED displays using the SH1106 chip.
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

#include <Adafruit_SH110X.h>  // Hardware-specific library for SH1106
#include <SPI.h>
#include <Wire.h>


class DisplaySH1106Adapter : public DisplayAdapterGFX {
public:
  bool start() override {
    display = new (std::nothrow) Adafruit_SH1106G(conf->width, conf->height, &Wire, conf->resetPin);

    if (!display) {
      LOGGER_ERR("not found");
      return (false);

    } else {
      gfxDisplay = (Adafruit_GFX *)display;
      display->begin(conf->i2cAddress);
      backColor565 = SH110X_BLACK;
      drawColor565 = SH110X_WHITE;
      DisplayAdapterGFX::start();
    }  // if
    return (true);
  };  // init()


  virtual void setBrightness(uint8_t bright) override {
    display->setContrast((bright * 255) / 100);
  };

  virtual void setColor(UNUSED uint32_t col) override {
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
  Adafruit_SH1106G *display = nullptr;
};
