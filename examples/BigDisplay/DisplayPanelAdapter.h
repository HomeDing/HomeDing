/**
 * @file DisplayPanelAdapter.h
 *
 * @brief DisplayAdapter implementation for the HomeDing library
 * adapting TFT displays using the Panel chip.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * -----
 * * 25.07.2018 created by Matthias Hertel
 * * 07.12.2020 no write text beyond textline end.
 */

#pragma once

#include <displays/DisplayAdapterGFX.h>

#include "GFX_ESP32Panel.h"

class DisplayPanelAdapter : public DisplayAdapterGFX {
public:
  ~DisplayPanelAdapter() = default;

  bool start() override {

#if defined(ESP32)
    display = new (std::nothrow) GFX_ESP32Panel(conf->width, conf->height);

#else
#error "GFX_ESP32Panel only supported by ESP32S3"
#endif

    if (!display) {
      LOGGER_ERR("not found");
      return (false);

    } else {
      gfxDisplay = (Adafruit_GFX *)display;
      DisplayAdapterGFX::start();
      _setTextHeight(24);
    }  // if
    return (true);
  };  // init()


private:
  /**
   * @brief Reference to the used library object
   */
  GFX_ESP32Panel *display = nullptr;
};
