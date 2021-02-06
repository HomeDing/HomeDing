/**
 * @file DisplayAdapter.h
 *
 * @brief Abstract class, defining the functionlaity the HomeDing requires to be
 * implemented for a local attached display.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license.
 * See https://www.mathertel.de/License.aspx.
 * More information on https://www.mathertel.de/Arduino
 * -----
 * 25.07.2018 created by Matthias Hertel
 * 23.09.2018 flush() method added.
 */

#ifndef DisplayAdapter_H
#define DisplayAdapter_H

#include <Wire.h>
#include <WireUtils.h>

#include <Board.h>

class DisplayAdapter
{
public:
  DisplayAdapter()
  {
    _lineHeight = 1;
    _charWidth = 1;
  } // DisplayAdapter()

  /**
   * @brief start the display.
   * @return true when display is ready for operation.
   * @return false otherwise.
   */
  virtual bool init(UNUSED Board *board)
  {
    return (true);
  };

  /**
   * @brief Clear the complete display
   */
  virtual void clear(){};

  virtual void setLineHeight(int16_t lh)
  {
    _lineHeight = lh;
  };

  virtual int getFontHeight(int fontsize)
  {
    return (fontsize);
  };

  virtual int16_t getLineHeight()
  {
    return (_lineHeight);
  };


  virtual void setCharWidth(int16_t cw)
  {
    _charWidth = cw;
  };

  virtual int16_t getCharWidth()
  {
    return (_charWidth);
  };

  virtual void setBrightness(UNUSED uint8_t bright) {};

  /**
   * @brief Clear a position or region.
   * @param x
   * @param y
   * @param w
   * @param h
   */
  virtual void clear(UNUSED int16_t x, UNUSED int16_t y, UNUSED int16_t w, UNUSED int16_t h){};

  // virtual void setColor(int col) {};

  virtual int drawText(int16_t x, int16_t y, int16_t h, String &text)
  {
    return (drawText(x, y, h, text.c_str()));
  };

  virtual int drawText(UNUSED int16_t x, UNUSED int16_t y, UNUSED int16_t h, const char *text)
  {
    return (_charWidth * strlen(text));
  };

  virtual void drawLine(UNUSED int16_t x0, UNUSED int16_t y0, UNUSED int16_t x1, UNUSED int16_t y1){};

  virtual int drawDot(UNUSED int16_t x, UNUSED int16_t y, int16_t h, UNUSED bool fill)
  {
    return (h);
  };

  /**
   * @brief The flush method must be called after every output sequence to allow
   * combined sending new information to the display.
   */
  virtual void flush(){};

private:
  // define the default line height
  int16_t _lineHeight;

  // define the default character width
  int16_t _charWidth;
};

#endif // DisplayAdapter_H