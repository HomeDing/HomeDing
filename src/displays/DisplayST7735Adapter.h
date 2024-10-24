/**
 * @file DisplayST7735Adapter.h
 *
 * @brief DisplayAdapter implementation for the HomeDing library
 * adapting LCD displays using the ST7735 chip.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * -----
 * * 21.10.2022 created by Matthias Hertel
 * * 07.06.2023 migrated from Adafruit_GFX to Arduino_GFX
 */

#pragma once

#include <displays/DisplayAGFXAdapter.h>

class DisplayST7735Adapter : public DisplayAGFXAdapter {
public:
  ~DisplayST7735Adapter() = default;

  bool start() override {
    bus = getBus();

    gfx = new Arduino_ST7735(
      bus,                    // bus
      displayConfig.resetPin,         // reset
      (displayConfig.rotation / 90),  // rotation
      displayConfig.ips,              // ips
      displayConfig.width,            // width
      displayConfig.height,           // height
      displayConfig.colOffset,        // display offset in memory
      displayConfig.rowOffset,
      displayConfig.colOffset,
      displayConfig.rowOffset,
      true);  //  bgr mode

    DisplayAGFXAdapter::start();

    return (gfx != nullptr);
  };  // init()
};
