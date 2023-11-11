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
      bus = new Arduino_ESP8266SPI(2 /* DC */, 15 /* CS */);
#endif
    }

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
      conf->rowOffset
    );

    DisplayAGFXAdapter::start();

    return (gfx != nullptr);
  };  // start()
};
