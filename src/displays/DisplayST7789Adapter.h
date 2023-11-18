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

    bus = getBus(conf);

    gfx = new Arduino_ST7789(
      bus,
      conf->resetPin,
      (conf->rotation / 90),
      conf->ips,
      conf->width,
      conf->height,
      conf->colOffset,
      conf->rowOffset,
      conf->colOffset,
      conf->rowOffset);

    DisplayAGFXAdapter::start();

    return (gfx != nullptr);
  };  // start()
};
