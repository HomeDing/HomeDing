// -----
// DisplayAdapterSSD1306.h
//
// Copyright (c) by Matthias Hertel, https://www.mathertel.de.
//
// This work is licensed under a BSD style license.
// See https://www.mathertel.de/License.aspx.
// More information on https://www.mathertel.de/Arduino
// -----
// 25.07.2018 created by Matthias Hertel
// -----


#ifndef DisplayAdapterSSD1306_H
#define DisplayAdapterSSD1306_H

#include "DisplayAdapter.h"
#include <Arduino.h>

#include <SSD1306Wire.h>

class DisplayAdapterSSD1306 : DisplayAdapter
{
public:
  /**
   * @brief Construct a new Display Adapter for a SSD1306 display
   * using common parameters.
   * I2C defaults to 4(SDA) and 5(SCL) for most boards.
   */
  DisplayAdapterSSD1306()
  {
    // use parameters of a common display board.
    DisplayAdapterSSD1306(0x3c, 5, 4, 64);
  } // DisplayAdapterSSD1306()


  /**
   * @brief Construct a new Display Adapter for a SSD1306 display
   * using specific parameters.
   */
  DisplayAdapterSSD1306(int address, int sda, int scl, int h)
      : _address(address), _sda(sda), _scl(scl)
  {
    if (h == 32)
      _resolution = GEOMETRY_128_32;
    else if (h == 64)
      _resolution = GEOMETRY_128_64;
  } // DisplayAdapterSSD1306()


  bool init()
  {
    // test if a display device is attached
    Wire.begin(_sda, _scl);
    Wire.beginTransmission(_address);
    int error = Wire.endTransmission();

    if (error != 0) {
      Serial.printf("display error %d\n", error);
      display = NULL;
      return (false);

    } else {
      Serial.printf("setupDisplay...\n");
      display = new SSD1306Wire(_address, _sda, _scl, _resolution);

      display->init();
      display->connect();

      display->flipScreenVertically();
      display->setTextAlignment(TEXT_ALIGN_LEFT);
      display->setFont(ArialMT_Plain_10);

      display->drawString(0, 0, "HomeDing...");
      display->display();
    } // if
    return (true);
  }; // init()


  /**
   * @brief Clear all displayed information from the display.
   */
  void clear()
  {
    display->clear();
    display->display();
  }; // clear()


  /**
   * @brief Clear information from the display in this area.
   * @param x x-position or offset of the text.
   * @param y y-position of the area.
   * @param w width of the area.
   * @param h height of the area.
   */
  void clear(int16_t x, int16_t y, int16_t w, int16_t h)
  {
    display->setColor(BLACK);
    display->fillRect(x, y, w, h);
    display->display();
  }; // clear()


  /**
   * @brief Draw a text at this position using the specific height.-
   * @param x x-position or offset of the text.
   * @param y y-position of the text.
   * @param h height of the characters
   * @param text the text.
   */
  void drawText(int16_t x, int16_t y, int16_t h, String &text)
  {
    if (h == 10) {
      display->setFont(ArialMT_Plain_10);
    } else if (h == 16) {
      display->setFont(ArialMT_Plain_16);
    } else if (h == 24) {
      display->setFont(ArialMT_Plain_24);
    } // if

    display->setColor(WHITE);
    display->drawString(x, y, text);
    display->display();
  }; // drawText()


  void drawText(int16_t x, int16_t y, int16_t h, char *text)
  {
    String s_text(text);
    drawText(x, y, h, s_text);
  }


  void drawDot(int16_t x, int16_t y, int16_t h, bool fill)
  {
    int r = h / 2;

    display->setColor(WHITE);
    if (fill) {
      display->fillCircle(x + r, y + r, r);
    } else {
      display->drawCircle(x + r, y + r, r);
    }
    display->display();
  }; // drawDot()

private:
  /**
   * @brief Reference to the used library object
   */
  SSD1306Wire *display;

  /**
   * @brief I2C Deisplay device address.
   */
  int _address;

  /**
   * @brief GPIO pin for the I2C SCL Line.
   */
  int _scl;

/**
   * @brief GPIO pin for the I2C SDA Line.
 */
  int _sda;

  /**
   * @brief Number of vertical pixels of the display.
   * The chip supports 32 and 64 vertical pixels.
   */
  OLEDDISPLAY_GEOMETRY _resolution;
};

#endif // DisplayAdapterSSD1306_H