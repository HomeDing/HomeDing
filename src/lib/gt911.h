// GT911 Touch Panel functions

// configuration information is read and written to registers 0x8047 to 0x80FF always at once
// using the _config buffer.

// The implementation was assembled using information from the data sheet and implementaton manual
// as well as from various implementation found on github.
// * https://www.goodix.com/en/product/touch/touch_screen_controller
// * https://github.com/goodix/goodix_gt9xx_public
//
// https://github.com/lvgl/lvgl_esp32_drivers/blob/master/lvgl_touch/gt911.h
//
// Changelog:
// * 19.11.2023 created by Matthias Hertel
// * 14.01.2024 supporting touch controllers with no Reset connection.

#include "Arduino.h"
#include <Wire.h>

#pragma once

#define GT911_MAX_CONTACTS 5


// Real-time command (Write only)
#define GT911_COMMAND (uint16_t)0x8040
#define GT911_ESD_CHECK (uint16_t)0x8041
#define GT911_COMMAND_CHECK (uint16_t)0x8046
#define GT911_CONFIG_SIZE (uint16_t)0xFF - 0x46

struct GDTpoint_t {
  uint8_t trackId;
  uint8_t area;
  uint16_t x;
  uint16_t y;
};

class GT911 {
public:
  GT911(int _int, int _rst, uint16_t _width, uint16_t _height);

  void init(int address = 0x5D);

  // change configuration

  void setResolution(uint16_t _width, uint16_t _height);

  // void setOnRead(void (*isr)());
  // uint8_t getGesture(void);

  /**
   * @brief Check if a touch event is detected and get the touch points.
   * @param points The array containing the coordinates of the touch points.
   * @return uint8_t The number of detected touch points.
   */
  uint8_t getTouchPoints(GDTpoint_t* points);

private:
  // static void ARDUINO_ISR_ATTR onInterrupt();
  // void (*onRead)();
  uint8_t addr;
  int pinInt;
  int pinRst;
  uint16_t width;
  uint16_t height;
  bool needReflash = false;
  uint8_t configBuf[GT911_CONFIG_SIZE];

};
