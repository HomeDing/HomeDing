/**
 * @file DisplayST7701Adapter.h
 *
 * @brief Display Element for ST7701 based TFT displays using the ESP32-S3 specific
 * 16-bit Panel Data interface.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * -----
 * * 27.01.2023 created by Matthias Hertel
 */

#pragma once

#include <displays/DisplayAGFXAdapter.h>

class DisplayST7701Adapter : public DisplayAGFXAdapter {

  bool start() override {

#if defined(CONFIG_IDF_TARGET_ESP32S3)

    int pinCount = ListUtils::length(displayConfig.busPins);
    int8_t pins[16];

    if (pinCount != 16) {
      LOGGER_ERR("ESP32Panel requires 16 pin definitions");
    } else {
      for (int n = 0; n < 16; n++) {
        pins[n] = Element::_atopin(ListUtils::at(displayConfig.busPins, n).c_str());
      }
    }

#if 0
    // from: <https://github.com/moononournation/Arduino_GFX/wiki/Dev-Device-Declaration#esp32-8048s043>
    Arduino_ESP32RGBPanel *rgbpanel = new Arduino_ESP32RGBPanel(
      40 /* DE */, 41 /* VSYNC */, 39 /* HSYNC */, 42 /* PCLK */,
      pins[0], pins[1], pins[2], pins[3], pins[4], pins[5], pins[6], pins[7],
      pins[8], pins[9], pins[10], pins[11], pins[12], pins[13], pins[14], pins[15],

      0 /* hsync_polarity */, 18 /* hsync_front_porch */, 4 /* hsync_pulse_width */, 8 /* hsync_back_porch */,
      0 /* vsync_polarity */, 18 /* vsync_front_porch */, 4 /* vsync_pulse_width */, 8 /* vsync_back_porch */
    );

    gfx = new Arduino_RGB_Display(
      800 /* width */, 480 /* height */, rgbpanel, 0 /* rotation */, false /* auto_flush */);

#else

    // similar to st7701_type1_init_operations, changes marked with !!

    static const uint8_t st7701_4848S040_init[] = {
      BEGIN_WRITE,
      WRITE_COMMAND_8, 0xFF,
      WRITE_BYTES, 5, 0x77, 0x01, 0x00, 0x00, 0x10,

      WRITE_C8_D16, 0xC0, 0x3B, 0x00,
      WRITE_C8_D16, 0xC1, 0x0D, 0x02,
      WRITE_C8_D16, 0xC2, 0x31, 0x05,
      WRITE_C8_D8, 0xCD, 0x00,  // 0xCD, 0x08 !!

      WRITE_COMMAND_8, 0xB0,  // Positive Voltage Gamma Control
      WRITE_BYTES, 16,
      0x00, 0x11, 0x18, 0x0E, 0x11, 0x06, 0x07, 0x08,
      0x07, 0x22, 0x04, 0x12, 0x0F, 0xAA, 0x31, 0x18,

      WRITE_COMMAND_8, 0xB1,  // Negative Voltage Gamma Control
      WRITE_BYTES, 16,
      0x00, 0x11, 0x19, 0x0E, 0x12, 0x07, 0x08, 0x08,
      0x08, 0x22, 0x04, 0x11, 0x11, 0xA9, 0x32, 0x18,

      // PAGE1
      WRITE_COMMAND_8, 0xFF,
      WRITE_BYTES, 5, 0x77, 0x01, 0x00, 0x00, 0x11,

      WRITE_C8_D8, 0xB0, 0x60,  // Vop=4.7375v
      WRITE_C8_D8, 0xB1, 0x32,  // VCOM=32
      WRITE_C8_D8, 0xB2, 0x07,  // VGH=15v
      WRITE_C8_D8, 0xB3, 0x80,
      WRITE_C8_D8, 0xB5, 0x49,  // VGL=-10.17v
      WRITE_C8_D8, 0xB7, 0x85,
      WRITE_C8_D8, 0xB8, 0x21,  // AVDD=6.6 & AVCL=-4.6
      WRITE_C8_D8, 0xC1, 0x78,
      WRITE_C8_D8, 0xC2, 0x78,

      WRITE_COMMAND_8, 0xE0,
      WRITE_BYTES, 3, 0x00, 0x1B, 0x02,

      WRITE_COMMAND_8, 0xE1,
      WRITE_BYTES, 11,
      0x08, 0xA0, 0x00, 0x00,
      0x07, 0xA0, 0x00, 0x00,
      0x00, 0x44, 0x44,

      WRITE_COMMAND_8, 0xE2,
      WRITE_BYTES, 12,
      0x11, 0x11, 0x44, 0x44,
      0xED, 0xA0, 0x00, 0x00,
      0xEC, 0xA0, 0x00, 0x00,

      WRITE_COMMAND_8, 0xE3,
      WRITE_BYTES, 4, 0x00, 0x00, 0x11, 0x11,

      WRITE_C8_D16, 0xE4, 0x44, 0x44,

      WRITE_COMMAND_8, 0xE5,
      WRITE_BYTES, 16,
      0x0A, 0xE9, 0xD8, 0xA0, 0x0C, 0xEB, 0xD8, 0xA0,
      0x0E, 0xED, 0xD8, 0xA0, 0x10, 0xEF, 0xD8, 0xA0,

      WRITE_COMMAND_8, 0xE6,
      WRITE_BYTES, 4, 0x00, 0x00, 0x11, 0x11,

      WRITE_C8_D16, 0xE7, 0x44, 0x44,

      WRITE_COMMAND_8, 0xE8,
      WRITE_BYTES, 16,
      0x09, 0xE8, 0xD8, 0xA0,
      0x0B, 0xEA, 0xD8, 0xA0,
      0x0D, 0xEC, 0xD8, 0xA0,
      0x0F, 0xEE, 0xD8, 0xA0,

      WRITE_COMMAND_8, 0xEB,
      WRITE_BYTES, 7, 0x02, 0x00, 0xE4, 0xE4, 0x88, 0x00, 0x40,

      WRITE_C8_D16, 0xEC, 0x3C, 0x00,

      WRITE_COMMAND_8, 0xED,
      WRITE_BYTES, 16,
      0xAB, 0x89, 0x76, 0x54, 0x02, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0x20, 0x45, 0x67, 0x98, 0xBA,

      //-----------VAP & VAN---------------
      WRITE_COMMAND_8, 0xFF,
      WRITE_BYTES, 5, 0x77, 0x01, 0x00, 0x00, 0x13,

      WRITE_C8_D8, 0xE5, 0xE4,

      WRITE_COMMAND_8, 0xFF,
      WRITE_BYTES, 5, 0x77, 0x01, 0x00, 0x00, 0x00,

      // WRITE_COMMAND_8, 0x21,   // 0x20 normal, 0x21 IPS !!
      WRITE_C8_D8, 0x3A, 0x60,  // 0x70 RGB888, 0x60 RGB666, 0x50 RGB565
      WRITE_COMMAND_8, 0x11,    // Sleep Out
      END_WRITE,

      DELAY, 120,

      BEGIN_WRITE,
      WRITE_COMMAND_8, 0x29,  // Display On
      END_WRITE
    };

    // 9-bit mode SPI
    bus = getBus();

    // panel connections (hardware specific)
    Arduino_ESP32RGBPanel *rgbpanel = new Arduino_ESP32RGBPanel(
      18 /* DE */, 17 /* VSYNC */, 16 /* HSYNC */, 21 /* PCLK */,
      pins[0], pins[1], pins[2], pins[3], pins[4], pins[5], pins[6], pins[7],
      pins[8], pins[9], pins[10], pins[11], pins[12], pins[13], pins[14], pins[15],

      1 /* hsync_polarity */, 10 /* hsync_front_porch */, 8 /* hsync_pulse_width */, 50 /* hsync_back_porch */,
      1 /* vsync_polarity */, 10 /* vsync_front_porch */, 8 /* vsync_pulse_width */, 20 /* vsync_back_porch */);

    // panel parameters & setup
    gfx = new Arduino_RGB_Display(
      displayConfig.width, displayConfig.height, rgbpanel, (displayConfig.rotation / 90),
      true /* auto_flush */,
      bus, displayConfig.resetPin,
      st7701_4848S040_init, sizeof(st7701_4848S040_init));

#endif

    gfx->invertDisplay(1);

    DisplayAGFXAdapter::start();
#endif

    return (gfx != nullptr);
  };  // start()
};
