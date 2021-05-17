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

#include <OLEDDisplay.h>
#include <displays/DisplayAdapter.h>

class DisplayAdapterOLED : public DisplayAdapter
{
public:
  /**
   * @brief Construct a new Display Adapter for a SH1106 display
   * using specific parameters.
   */
  DisplayAdapterOLED()
  {
    setLineHeight(10);
    setCharWidth(8);
  } // DisplayAdapterSH1106()


  virtual ~DisplayAdapterOLED() = default;


  virtual bool init(UNUSED Board *board, OLEDDisplay *d)
  {
    display = d;
    d->init();
    hd_yield();
    d->flipScreenVertically();
    d->setTextAlignment(TEXT_ALIGN_LEFT);
    d->setFont(ArialMT_Plain_10);
    hd_yield();

    d->clear();
    d->display();
    hd_yield();

    return (true);
  }; // init()


  virtual void setBrightness(uint8_t bright) override
  {
    display->setBrightness(bright);
  };

  /**
   * @brief Clear all displayed information from the display.
   */
  void clear()
  {
    display->clear();
    _dirty = true;
  }; // clear()


  int getFontHeight(int fontsize) override
  {
    int h = 0;
    const uint8_t *fontData = _getFontData(fontsize);
    if (fontData) {
      h = pgm_read_byte(fontData + HEIGHT_POS);
    }
    return (h);
  };

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
   * @return width of drawn text.
   */
  int drawText(int16_t x, int16_t y, int16_t h, const char *text)
  {
    String tmp(text);
    const uint8_t *fontData = _getFontData(h);
    int w = 0;

    if (fontData) {
      display->setFont(fontData);
      display->setColor(WHITE);
      display->drawString(x, y, tmp);
      w = display->getStringWidth(tmp);
      _dirty = true;
    }
    return (w);
  } // drawText()


  void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
  {
    display->setColor(WHITE);
    display->drawLine(x0, y0, x1, y1);
    _dirty = true;
  } // drawLine()


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

private:
  /**
   * @brief Reference to the used library object
   */
  OLEDDisplay *display = NULL;

  bool _dirty;

  /**
   * @brief Number of vertical pixels of the display.
   * The chip supports 32 and 64 vertical pixels.
   */
  OLEDDISPLAY_GEOMETRY _resolution;

  const uint8_t *_getFontData(int fontsize)
  {
    const uint8_t *data = nullptr;
    if ((fontsize == 0) || (fontsize == 10)) {
      data = ArialMT_Plain_10;
    } else if (fontsize == 16) {
      data = ArialMT_Plain_16;
    } else if (fontsize == 24) {
      data = ArialMT_Plain_24;
    }
    return (data);
  };
};

#endif // DisplayAdapterSH1106_H