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
 * * 05.11.2023 reworked for Arduino_GFX compatible driver
 * * 05.11.2023 Arduino_GFX compatible driver donated to https://github.com/moononournation/Arduino_GFX/pull/376
 */

#pragma once

#include <displays/DisplayAGFXAdapter.h>

class DisplaySH1106Adapter : public DisplayAGFXAdapter {

public:
  bool start() override {
    PANELTRACE("DisplaySH1106Adapter.start()\n");

    displayConfig.busmode = BUSMODE_I2C;
    displayConfig.i2cCommandPrefix = 0x00;
    displayConfig.i2cDataPrefix = 0x40;
    bus = getBus();

    if (bus) {
      // Initialize the display using the physical parameters
      // and use the Mono color Canvas for drawing
      op = new Arduino_SH1106(bus, displayConfig.resetPin, displayConfig.width, displayConfig.height);
      gfx = new Arduino_Canvas_Mono(displayConfig.width, displayConfig.height, op, 0, 0, true);

      // set rotatation for drawing.
      gfx->setRotation(displayConfig.rotation / 90);
      // displayConfig.rotation
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
  Arduino_SH1106 *op = nullptr;
};
