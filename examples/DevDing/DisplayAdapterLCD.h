/**
 * @file DisplayAdapterLCD.h
 *
 * @brief DisplayAdapter implementation for the HomeDing library
 * adapting LCD displays using the HD44780 chip.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * -----
 * * 25.07.2018 created by Matthias Hertel
 */

#ifndef DisplayAdapterLCD_H
#define DisplayAdapterLCD_H

#include "DisplayAdapter.h"
#include <Arduino.h>

#include <LiquidCrystal_PCF8574.h>

class DisplayAdapterLCD : DisplayAdapter
{
public:
  /**
   * @brief Construct a new Display Adapter for a SSD1306 display
   * using common parameters.
   * I2C defaults to 4(SDA) and 5(SCL) for most boards.
   */
  DisplayAdapterLCD()
  {
    // use parameters of a common display board.
    DisplayAdapterLCD(0x27, SDA, SCL);
  } // DisplayAdapterLCD()


  /**
   * @brief Construct a new Display Adapter for a SSD1306 display
   * using specific parameters.
   */
  DisplayAdapterLCD(int address, int sda, int scl)
      : _address(address), _sda(sda), _scl(scl)
  {
  } // DisplayAdapterLCD()


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
      display = new LiquidCrystal_PCF8574(_address);

      display->begin(16, 2);
      display->clear();
      display->setCursor(0, 0);
      display->print("HomeDing...");
      delay(800);
    } // if
    return (true);
  }; // init()


  /**
   * @brief Clear all displayed information from the display.
   */
  void clear()
  {
    display->clear();
  }; // clear()


  /**
   * @brief Clear information from the display in this area.
   * @param x x-position or offset of the text.
   * @param y y-position of the area.
   * @param w width of the area.
   * @param h height of the area, assumed always 1.
   */
  void clear(int16_t x, int16_t y, int16_t w, int16_t h)
  {
    return;
    if ((x > 20) || (y > 4)) {
      Serial.printf("outside\n");
    } else {
      display->setCursor(x, y);
      // while (w > 0) {
        display->write('#');
        w--;
      // }
    } // if
  }; // clear()


  /**
   * @brief Draw a text at this position using the specific height.-
   * @param x x-position or offset of the text.
   * @param y y-position of the text.
   * @param h height of the characters
   * @param text the text.
   */
  void drawText(int16_t x, int16_t y, int16_t h, const char *text)
  {
    if ((x > 20) || (y > 4)) {
      Serial.printf("outside\n");
    } else {
      display->setCursor(x, y);
      display->print(text);
    }
  }


  void drawText(int16_t x, int16_t y, int16_t h, String &text)
  {
    drawText(x, y, 1, text.c_str());
  }; // drawText()


  void drawDot(int16_t x, int16_t y, int16_t h, bool fill)
  {
    x=14; y=1;
    if (fill) {
      drawText(x, y, 1, "+");
    } else {
      drawText(x, y, 1, "-");
    }
  }; // drawDot()

private:
  /**
   * @brief Reference to the used library object
   */
  LiquidCrystal_PCF8574 *display;

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
};

#endif // DisplayAdapterLCD_H