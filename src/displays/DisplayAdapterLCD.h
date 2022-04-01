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
 * * 07.12.2020 no write text beyond textline end.
 */

#ifndef DisplayAdapterLCD_H
#define DisplayAdapterLCD_H

#include <LiquidCrystal_PCF8574.h>
#include <displays/DisplayAdapter.h>

class DisplayAdapterLCD : public DisplayAdapter {
public:
  bool start() override {
    // test if a device is attached
    if (!WireUtils::exists(conf->i2cAddress)) {
      LOGGER_ERR("not found");
      return (false);

    } else {
      display = new (std::nothrow) LiquidCrystal_PCF8574(conf->i2cAddress);

      if (!display) {
        LOGGER_ERR("not found");
        return (false);

      } else {
        DisplayAdapter::start();

        display->begin(conf->width, conf->height);
        lineHeight = 1;
        charWidth = 1;

        byte dotOff[] = { 0b00000, 0b01110, 0b10001, 0b10001,
                          0b10001, 0b01110, 0b00000, 0b00000 };
        byte dotOn[] = { 0b00000, 0b01110, 0b11111, 0b11111,
                         0b11111, 0b01110, 0b00000, 0b00000 };

        display->createChar(1, dotOff);
        display->createChar(2, dotOn);

        display->clear();
        display->setBacklight(1);
      }

    }  // if
    return (true);
  };  // init()


  /**
   * @brief Clear all displayed information from the display.
   */
  void clear() override {
    display->clear();
  };  // clear()


  /**
   * @brief Clear information from the display in this area.
   * @param x x-position or offset of the text.
   * @param y y-position of the area.
   * @param w width of the area.
   * @param h height of the area, assumed always 1.
   */
  void clear(int16_t x, int16_t y, int16_t w, UNUSED int16_t h) override {
    display->setCursor(x, y);
    if (y < conf->height) {
      while ((x < conf->width) && (w > 0)) {
        display->write(' ');
        w--;
        x++;
      }  // while
    }    // if
  };     // clear()


  /**
   * @brief Draw a text at this position using the specific height.-
   * @param x x-position or offset of the text.
   * @param y y-position of the text.
   * @param h height of the characters, ignored for this display.
   * @param text the text.
   */
  int drawText(int16_t x, int16_t y, UNUSED int16_t h, const char *text) override {
    int w = strnlen(text, MAX_DISPLAY_STRING_LEN);
    char buffer[MAX_DISPLAY_STRING_LEN + 4];  // 8 chars character buffer max.
    if ((x > 20) || (y > 4)) {
      // TRACE("outside");
    } else {
      strlcpy(buffer, text, MAX_DISPLAY_STRING_LEN);
      buffer[conf->width - x] = '\0';
      display->setCursor(x, y);
      display->print(buffer);
    }
    return (w);
  }  // drawText


  int drawDot(int16_t x, int16_t y, UNUSED int16_t h, bool fill) override {
    drawText(x, y, 1, fill ? "\02" : "\01");
    return (1);
  };  // drawDot()

private:
  /**
   * @brief Reference to the used library object
   */
  LiquidCrystal_PCF8574 *display = NULL;
};

#endif  // DisplayAdapterLCD_H