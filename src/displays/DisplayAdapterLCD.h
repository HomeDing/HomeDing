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

class DisplayAdapterLCD : public DisplayAdapter
{
public:
  /**
   * @brief Construct a new Display Adapter for a LiquidCrystal display
   * using common address 0x27 and 2*16 layout
   */
  DisplayAdapterLCD()
  {
    // use parameters of a common display board.
    DisplayAdapterLCD(0x27, 2, 16);
  } // DisplayAdapterLCD()


  virtual ~DisplayAdapterLCD() = default;


  /**
   * @brief Construct a new Display Adapter for a SSD1306 display
   * using specific parameters.
   */
  DisplayAdapterLCD(int address, int lines, int columns)
      : _address(address), _lines(lines), _cols(columns)
  {
    setLineHeight(1);
    setCharWidth(1);

  } // DisplayAdapterLCD()


  bool init(Board *board)
  {
    // test if a device is attached
    if (!WireUtils::exists(_address)) {
      LOGGER_ERR("not found");
      return (false);

    } else {
      display = new (std::nothrow) LiquidCrystal_PCF8574(_address);

      if (!display) {
        LOGGER_ERR("not found");
        return (false);

      } else {
        DisplayAdapter::init(board);

        display->begin(_cols, _lines);

        byte dotOff[] = {0b00000, 0b01110, 0b10001, 0b10001,
                         0b10001, 0b01110, 0b00000, 0b00000};
        byte dotOn[] = {0b00000, 0b01110, 0b11111, 0b11111,
                        0b11111, 0b01110, 0b00000, 0b00000};

        display->createChar(1, dotOff);
        display->createChar(2, dotOn);

        display->clear();
        display->setBacklight(1);
      }

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


  int getFontHeight(UNUSED int fontsize) override {
    return(1);
  };

  /**
   * @brief Clear information from the display in this area.
   * @param x x-position or offset of the text.
   * @param y y-position of the area.
   * @param w width of the area.
   * @param h height of the area, assumed always 1.
   */
  void clear(int16_t x, int16_t y, int16_t w, UNUSED int16_t h)
  {
    display->setCursor(x, y);
    if (y < _lines) {
      while ((x < _cols) && (w > 0)) {
        display->write(' ');
        w--;
        x++;
      } // while
    } // if
  }; // clear()


  /**
   * @brief Draw a text at this position using the specific height.-
   * @param x x-position or offset of the text.
   * @param y y-position of the text.
   * @param h height of the characters, ignored for this display.
   * @param text the text.
   */
  int drawText(int16_t x, int16_t y, UNUSED int16_t h, const char *text)
  {
    int w = strlen(text);
    char buffer[80+4]; // 8 chars character buffer max.
    if ((x > 20) || (y > 4)) {
      Serial.printf("outside\n");
    } else {
      strncpy(buffer, text, 80);
      buffer[_cols-x] = '\0';
      display->setCursor(x, y);
      display->print(buffer);
    }
    return (w);
  } // drawText


  int drawDot(int16_t x, int16_t y, UNUSED int16_t h, bool fill)
  {
    drawText(x, y, 1, fill ? "\02" : "\01");
    return (1);
  }; // drawDot()

private:
  /**
   * @brief Reference to the used library object
   */
  LiquidCrystal_PCF8574 *display = NULL;

  /**
   * @brief I2C Deisplay device address.
   */
  int _address;

  int _lines;
  int _cols;
};

#endif // DisplayAdapterLCD_H