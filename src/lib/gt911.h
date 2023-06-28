// GT911 Touch Panel functions

// configuration information is read and written to registers 0x8047 to 0x80FF alwasy at once
// using the _config buffer.

// https://www.goodix.com/en/product/touch/touch_screen_controller
// https://github.com/goodix

// https://github.com/lvgl/lvgl_esp32_drivers/blob/master/lvgl_touch/gt911.h

#include "Arduino.h"
#include <Wire.h>

#pragma once

#define GT911_MAX_CONTACTS 5


// Real-time command (Write only)
#define GT911_COMMAND       (uint16_t)0x8040
#define GT911_ESD_CHECK     (uint16_t)0x8041
#define GT911_COMMAND_CHECK (uint16_t)0x8046
#define GT911_CONFIG_SIZE              (uint16_t)0xFF-0x46

struct GDTpoint_t {
  uint8_t trackId;
  uint8_t area;
  uint16_t x;
  uint16_t y;
};

class GT911 {
  public:
    GT911(uint8_t _int, uint8_t _rst, uint16_t _width, uint16_t _height);

    void init(bool useAdr2 = false);
    void setRotation(int rotation);
 
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
    uint8_t _rotation = 0;
    uint8_t addr;
    uint8_t pinInt;
    uint8_t pinRst;
    uint16_t width;
    uint16_t height;
    uint8_t configBuf[GT911_CONFIG_SIZE];
    // uint8_t *configBuf;
};

