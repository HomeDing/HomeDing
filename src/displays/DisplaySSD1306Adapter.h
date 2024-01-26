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
 * * 05.11.2023 reworked for Arduino_GFX compatible driver
 * * 05.11.2023 Arduino_GFX compatible driver donated to https://github.com/moononournation/Arduino_GFX/pull/376
 */

#pragma once

#include <displays/DisplayAGFXAdapter.h>

class DisplaySSD1306Adapter : public DisplayAGFXAdapter {

public:
  bool start() override {
    PANELTRACE("DisplaySSD1306Adapter.start()\n");

    if (conf->busmode == BUSMODE_ANY) { conf->busmode = BUSMODE_I2C; }
    if (conf->busmode == BUSMODE_I2C) {
      conf->i2cCommandPrefix = 0x00;
      conf->i2cDataPrefix = 0x40;
    }
    bus = getBus(conf);

    if (bus) {
      // Initialize the display using the physical parameters and use the Mono color Canvas for drawing
      op = new Arduino_SSD1306(bus, conf->resetPin, conf->width, conf->height);
      gfx = new Arduino_Canvas_Mono(conf->width, conf->height, op, conf->colOffset, conf->rowOffset, true);

      // set rotatation for drawing.
      gfx->setRotation(conf->rotation / 90);
    }

    DisplayAGFXAdapter::start();
    return (gfx != nullptr);
  };  // start()


  /// @brief set brightness for panel lightning.
  /// @param bright new brightness in range 0...100
  virtual void setBrightness(uint8_t bright) override {
    if (op) { op->setBrightness((((uint16_t)bright) * 255) / 100); }
  }

private:
  Arduino_SSD1306 *op = nullptr;
};
