/**
 * @file DisplayAdapterSH1106.h
 *
 * @brief DisplayAdapter implementation for the HomeDing library
 * adapting OLED displays using the SH1106 chip.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * -----
 * * 25.07.2018 created by Matthias Hertel
 */

#ifndef DisplayAdapterOLED_H
#define DisplayAdapterOLED_H

#include <displays/DisplayAdapter.h>
#include <OLEDDisplay.h>

class DisplayAdapterOLED : DisplayAdapter
{
public:
  DisplayAdapterOLED() {
    // don't use this constructor
  }
  
  /**
   * @brief Construct a new Display Adapter for a SH1106 display
   * using specific parameters.
   */
  DisplayAdapterOLED(int address, int h)
      : _address(address)
  {
  if (h == 32)
      _resolution = GEOMETRY_128_32;
    else if (h == 64)
      _resolution = GEOMETRY_128_64;
    lineHeight = 10;
    charWidth = 8;
  } // DisplayAdapterSH1106()


  virtual bool init(Board *board, OLEDDisplay *d)
  {
    display = d;
    display->flipScreenVertically();
    display->setTextAlignment(TEXT_ALIGN_LEFT);
    display->setFont(ArialMT_Plain_10);

    display->clear();
    display->display();
    delay(1);

    return (true);
  }; // init()


  /**
   * @brief Clear all displayed information from the display.
   */
  void clear()
  {
    display->clear();
    _dirty = true;
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
    _dirty = true;
  }; // clear()


  /**
   * @brief Draw a text at this position using the specific height.-
   * @param x x-position or offset of the text.
   * @param y y-position of the text.
   * @param h height of the characters. Use 0 to use standard height;
   * @param text the text.
   */
  int drawText(int16_t x, int16_t y, int16_t h, const char *text)
  {
    String tmp(text);
    if ((h == 0) || (h == 10)) {
      display->setFont(ArialMT_Plain_10);
    } else if (h == 16) {
      display->setFont(ArialMT_Plain_16);
    } else if (h == 24) {
      display->setFont(ArialMT_Plain_24);
    } // if

    display->setColor(WHITE);
    display->drawString(x, y, tmp);
    _dirty = true;
    return (display->getStringWidth(tmp));
  }

  int drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
  {
    display->setColor(WHITE);
    display->drawLine(x0, y0, x1, y1);
    _dirty = true;
  };

  int drawDot(int16_t x, int16_t y, int16_t h, bool fill)
  {
    int r = h / 2;

    display->setColor(WHITE);
    if (fill) {
      display->fillCircle(x + r, y + r, r);
    } else {
      display->drawCircle(x + r, y + r, r);
    }
    return (h);
  }; // drawDot()


  void flush()
  {
    if (_dirty) {
      display->display();
      _dirty = false;
    }
  }; // flush()

protected:
  /**
   * @brief Reference to the used library object
   */
  OLEDDisplay *display = NULL;

  /**
   * @brief I2C Deisplay device address.
   */
  int _address;


  bool _dirty;

  /**
   * @brief Number of vertical pixels of the display.
   * The chip supports 32 and 64 vertical pixels.
   */
  OLEDDISPLAY_GEOMETRY _resolution;
};

#endif // DisplayAdapterSH1106_H