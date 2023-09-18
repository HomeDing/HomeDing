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

    // from: <https://github.com/moononournation/Arduino_GFX/wiki/Dev-Device-Declaration#esp32-8048s043>
    Arduino_ESP32RGBPanel *rgbpanel = new Arduino_ESP32RGBPanel(
      40 /* DE */, 41 /* VSYNC */, 39 /* HSYNC */, 42 /* PCLK */,
      45 /* R0 */, 48 /* R1 */, 47 /* R2 */, 21 /* R3 */, 14 /* R4 */,
      5 /* G0 */, 6 /* G1 */, 7 /* G2 */, 15 /* G3 */, 16 /* G4 */, 4 /* G5 */,
      8 /* B0 */, 3 /* B1 */, 46 /* B2 */, 9 /* B3 */, 1 /* B4 */,

      0 /* hsync_polarity */, 8 /* hsync_front_porch */, 4 /* hsync_pulse_width */, 8 /* hsync_back_porch */,
      0 /* vsync_polarity */, 8 /* vsync_front_porch */, 4 /* vsync_pulse_width */, 8 /* vsync_back_porch */
    );

    gfx = new Arduino_RGB_Display(
      800 /* width */, 480 /* height */, rgbpanel, 0 /* rotation */, true /* auto_flush */);

    DisplayAGFXAdapter::start();
#endif

    return (gfx != nullptr);
  };  // start()
};
