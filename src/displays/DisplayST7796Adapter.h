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

    if ((conf->busmode == BUSMODE_ANY) || (conf->busmode == BUSMODE_SPI)) {

#if defined(ESP32)
      bus = new Arduino_ESP32SPI(
        conf->spiDC,
        conf->spiCS,
        conf->spiCLK,
        conf->spiMOSI,
        conf->spiMISO,
        HSPI /* spi_num */
      );

#elif defined(ESP8266)
      // ESP8266 has pre-defined SPI pins
      bus = new Arduino_ESP8266SPI(
        conf->spiDC, conf->spiCS);
#endif

    }

    gfx = new Arduino_ST7796(
      bus,
      conf->spiRST,
      (conf->rotation / 90),
      conf->ips,
      conf->width,
      conf->height);

    DisplayAGFXAdapter::start();

    return (gfx != nullptr);
  }
};
