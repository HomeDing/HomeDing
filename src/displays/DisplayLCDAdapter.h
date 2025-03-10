/**
 * @file DisplayLCDAdapter.h
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

#pragma once

#include <LiquidCrystal_PCF8574.h>
#include <displays/DisplayAdapter.h>

class DisplayLCDAdapter : public DisplayAdapter {
public:
  bool start() override {
    // test if a device is attached
    if (!WireUtils::exists(HomeDing::displayConfig.i2cAddress)) {
      LOGGER_ERR("not found");
      return (false);

    } else {
      display = new (std::nothrow) LiquidCrystal_PCF8574(HomeDing::displayConfig.i2cAddress);

      if (!display) {
        LOGGER_ERR("not found");
        return (false);

      } else {
        DisplayAdapter::start();

        display->begin(HomeDing::displayConfig.width, HomeDing::displayConfig.height);

        // add some useful extra characters

        // #01 circle with no filling
        byte dotOff[] = { 0b00000, 0b01110, 0b10001, 0b10001, 0b10001, 0b01110, 0b00000, 0b00000 };
        display->createChar(1, dotOff);

        // #02 circle with filling
        byte dotOn[8] = { 0b00000, 0b01110, 0b11111, 0b11111, 0b11111, 0b01110, 0b00000, 0b00000 };
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


  /// @brief Draw a text at this position using the specific height.-
  /// @param x x-position or offset of the text.
  /// @param y y-position of the text.
  /// @param h height of the characters, ignored for this display.
  /// @param text the text.
  BoundingBox drawText(int16_t x, int16_t y, int16_t h, const char *text, uint32_t strokeColor) override {
    char buffer[MAX_DISPLAY_STRING_LEN + 4];  // 8 chars character buffer max.
    if ((x > 20) || (y > 4)) {
      // TRACE("outside");
    } else {
      strlcpy(buffer, text, MAX_DISPLAY_STRING_LEN);
      buffer[HomeDing::displayConfig.width - x] = '\0';
      display->setCursor(x, y);
      display->print(buffer);
    }
    BoundingBox b = DisplayAdapter::drawText(x, y, h, text, strokeColor);
    return (b);
  }  // drawText

private:
  /**
   * @brief Reference to the used library object
   */
  LiquidCrystal_PCF8574 *display = NULL;
};
