
#pragma once

#include <Adafruit_GFX.h>

///  A GFX 16-bit canvas context with external buffer option

class GFX_ESP32Panel : public Adafruit_GFX {
public:
  GFX_ESP32Panel(uint16_t w, uint16_t h);
  ~GFX_ESP32Panel(void);
  void startWrite() override;
  void endWrite() override;

  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void fillScreen(uint16_t color);
  void byteSwap(void);
  void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
  uint16_t getPixel(int16_t x, int16_t y) const;
  /**********************************************************************/
  /*!
    @brief    Get a pointer to the internal buffer memory
    @returns  A pointer to the allocated buffer
  */
  /**********************************************************************/
  uint16_t *getBuffer(void) const {
    return buffer;
  }

protected:
  uint16_t getRawPixel(int16_t x, int16_t y) const;
  void drawFastRawVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  void drawFastRawHLine(int16_t x, int16_t y, int16_t w, uint16_t color);

private:
  uint32_t bufferSize;
  uint16_t *buffer;
};
