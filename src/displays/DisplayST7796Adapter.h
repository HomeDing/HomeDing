/**
 * @file DisplayAGFXAdapter.h
 *
 * @brief DisplayAdapter implementation for the HomeDing library
 * adapting TFT displays using the Arduino_GFX library.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * -----
 * * 27.05.2023 created by Matthias Hertel
 */

#pragma once

#include <displays/DisplayAGFXAdapter.h>

class DisplayST7796Adapter : public DisplayAGFXAdapter {

  bool start() override {
    bus = getBus();

    if (bus) {
      gfx = new Arduino_ST7796(
        bus,
        displayConfig.resetPin,
        (displayConfig.rotation / 90),
        displayConfig.ips,
        displayConfig.width,
        displayConfig.height);
    }

    DisplayAGFXAdapter::start();

    return (gfx != nullptr);
  }
};
