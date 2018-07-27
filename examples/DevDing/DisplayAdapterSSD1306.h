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

// #include "DisplayAdapterSSD1306.h"
#include <Arduino.h>
#include "DisplayAdapter.h"

#include <SSD1306Wire.h>

class DisplayAdapterSSD1306 : DisplayAdapter
{
public:
  bool init()
  {
    uint8_t address = 0x3C;
    uint8_t sda = 5;
    uint8_t scl = 4;

    // test if a display device is attached
    Wire.begin(sda, scl);
    Wire.beginTransmission(address);
    int error = Wire.endTransmission();

    if (error != 0) {
      Serial.printf("display error %d\n", error);
      display = NULL;
      return (false);

    } else {
      Serial.printf("setupDisplay...\n");
      display = new SSD1306Wire(address, sda, scl, GEOMETRY_128_64);

      display->init();
      display->connect();

      display->flipScreenVertically();
      display->setTextAlignment(TEXT_ALIGN_LEFT);
      display->setFont(ArialMT_Plain_10);

      display->drawString(0, 0, "IoT...");
      display->display();
    } // if
    return (true);
  }; // init()


  void clear()
  {
    display->clear();
    display->display();
  }; // clear()


  void clear(int16_t x, int16_t y, int16_t w, int16_t h)
  {
    display->setColor(BLACK);
    display->fillRect(x, y, w, h);
    display->display();
  }; // clear()

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


  void drawText(int16_t x, int16_t y, int16_t h, char *text) {
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
  SSD1306Wire *display;

};

#endif // DisplayAdapterSSD1306_H