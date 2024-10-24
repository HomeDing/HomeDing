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

#include <displays/DisplayAGFXAdapter.h>

class DisplayST7789Adapter : public DisplayAGFXAdapter {

  bool start() override {

    bus = getBus();

    gfx = new Arduino_ST7789(
      bus,
      displayConfig.resetPin,
      (displayConfig.rotation / 90),
      displayConfig.ips,
      displayConfig.width,
      displayConfig.height,
      displayConfig.colOffset,
      displayConfig.rowOffset,
      displayConfig.colOffset,
      displayConfig.rowOffset);

    DisplayAGFXAdapter::start();

    return (gfx != nullptr);
  };  // start()
};
