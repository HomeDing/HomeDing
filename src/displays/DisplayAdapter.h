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

class DisplayAdapter
{
public:
  /**
   * @brief start the display.
   * @return true when display is ready for operation.
   * @return false otherwise.
   */
  virtual bool init();

  /**
   * @brief Clear the complete display
   */
  virtual void clear();

  // define the default line height
  int lineHeight;

  // define the default character width
  int charWidth;

  /**
   * @brief Clear a position or region.
   * @param x
   * @param y
   * @param w
   * @param h
   */
  virtual void clear(int16_t x, int16_t y, int16_t w, int16_t h);

  virtual int drawText(int16_t x, int16_t y, int16_t h, String &text);

  virtual int drawText(int16_t x, int16_t y, int16_t h, const char *text);

  virtual int drawDot(int16_t x, int16_t y, int16_t h, bool fill);

  /**
   * @brief The flush method must be called after every output sequence to allow
   * combined sending new information to the display.
   */
  virtual void flush();
};

#endif // DisplayAdapter_H