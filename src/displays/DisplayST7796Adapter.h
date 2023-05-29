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

#if defined(ESP32)
    bus = new Arduino_ESP32SPI(2 /* DC */, 15 /* CS */, 14 /* SCK */, 13 /* MOSI */, 12 /* MISO */, HSPI /* spi_num */);
#elif defined(ESP8266)
    bus = new Arduino_ESP8266SPI(2 /* DC */, 15 /* CS */);
#endif

    gfx = new Arduino_ST7796(
      bus,
      -1 /* RST */,
      (conf->rotation / 90) /* rotation */,
      false,
      conf->width,
      conf->height);

    DisplayAGFXAdapter::start();

    return (gfx != nullptr);
  }
};
