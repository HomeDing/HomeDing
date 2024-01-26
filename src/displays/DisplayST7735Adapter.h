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
    LOGGER_JUSTINFO("init: w:%d, h:%d, r:%d", conf->width, conf->height, conf->rotation);
    LOGGER_JUSTINFO("  pins: l:%d, r:%d", conf->lightPin, conf->resetPin);
    LOGGER_JUSTINFO("   i2c: adr:%d, sda:%d, scl:%d", conf->i2cAddress, conf->i2cSDA, conf->i2cSCL);
    LOGGER_JUSTINFO("   spi: cs:%d, dc:%d, mosi:%d, miso:%d, clk:%d", conf->csPin, conf->dcPin, conf->spiMOSI, conf->spiMISO, conf->spiCLK);

    if ((conf->busmode == BUSMODE_ANY) || (conf->busmode == BUSMODE_SPI)) {

#if defined(ESP32)

#if defined(CONFIG_IDF_TARGET_ESP32C3)
      bus = new Arduino_HWSPI(conf->dcPin, conf->csPin, conf->spiCLK, conf->spiMOSI, conf->spiMISO);

#elif defined(CONFIG_IDF_TARGET_ESP32S3)
      bus = new Arduino_ESP32SPI(
        conf->dcPin,
        conf->csPin,
        conf->spiCLK,
        conf->spiMOSI,
        conf->spiMISO,
        HSPI /* spi_num */
      );
#else
      bus = new Arduino_HWSPI(conf->dcPin, conf->csPin, conf->spiCLK, conf->spiMOSI, conf->spiMISO);
#endif

#elif defined(ESP8266)
      bus = new Arduino_HWSPI(conf->dcPin, conf->csPin);
#endif
    } else {
      // any bus but SPI is not implemented
    }

    gfx = new Arduino_ST7735(
      bus,                    // bus
      conf->resetPin,         // reset
      (conf->rotation / 90),  // rotation
      conf->ips,              // ips
      conf->width,            // width
      conf->height,           // height
      conf->colOffset,        // display offset in memory
      conf->rowOffset,
      conf->colOffset,
      conf->rowOffset,
      true);  //  bgr mode

    DisplayAGFXAdapter::start();

    return (gfx != nullptr);
  };  // init()
};
