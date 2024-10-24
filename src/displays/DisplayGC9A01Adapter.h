/**
 * @file DisplayGC9A01Adapter.h
 *
 * @brief DisplayAdapter implementation for the HomeDing library
 * adapting TFT displays using the GC9A01 chip.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * -----
 * * 11.11.2023 created by Matthias Hertel
**/

#pragma once

#include <displays/DisplayAGFXAdapter.h>

class DisplayGC9A01Adapter : public DisplayAGFXAdapter {

public:
  bool start() override {
    bus = getBus();

    if (bus) {
      gfx = new Arduino_GC9A01(
        bus,
        displayConfig.resetPin,
        (displayConfig.rotation / 90),
        displayConfig.ips,
        displayConfig.width,
        displayConfig.height);
    }
    
    DisplayAGFXAdapter::start();

    return (gfx != nullptr);
  };  // start()
};
