/**
 * @file DisplayESP32PanelAdapter.h
 *
 * @brief DisplayAdapter implementation for the HomeDing library
 * adapting OLED displays using the SSD1306 chip.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * -----
 * * 25.07.2018 created by Matthias Hertel
 * * 18.03.2022 based on Adafruit GFX driver
 * * 02.04.2023 using GFX.canvas1 and sending out data in adapter.
 */

#pragma once

#include <displays/DisplayAGFXAdapter.h>

class DisplayESP32PanelAdapter : public DisplayAGFXAdapter {

  bool start() override {

#if defined(ARDUINO_ESP32S3_DEV)

    // from: <https://github.com/moononournation/Arduino_GFX/wiki/Dev-Device-Declaration#esp32-8048s070>
    Arduino_ESP32RGBPanel *rgbpanel = new Arduino_ESP32RGBPanel(
      41 /* DE */, 40 /* VSYNC */, 39 /* HSYNC */, 42 /* PCLK */,
      14 /* R0 */, 21 /* R1 */, 47 /* R2 */, 48 /* R3 */, 45 /* R4 */,
      9 /* G0 */, 46 /* G1 */, 3 /* G2 */, 8 /* G3 */, 16 /* G4 */, 1 /* G5 */,
      15 /* B0 */, 7 /* B1 */, 6 /* B2 */, 5 /* B3 */, 4 /* B4 */,
      0 /* hsync_polarity */, 180 /* hsync_front_porch */, 30 /* hsync_pulse_width */, 16 /* hsync_back_porch */,
      0 /* vsync_polarity */, 12 /* vsync_front_porch */, 13 /* vsync_pulse_width */, 10 /* vsync_back_porch */);

    gfx = new Arduino_RGB_Display(
      800 /* width */, 480 /* height */, rgbpanel);

    DisplayAGFXAdapter::start();
#endif

    return (gfx != nullptr);
  };  // start()
};
