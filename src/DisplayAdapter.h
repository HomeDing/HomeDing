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
 */

#ifndef DisplayAdapter_H
#define DisplayAdapter_H

#include <Arduino.h>

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


/**
 * @brief Clear a position or region.
 * @param x 
 * @param y 
 * @param w 
 * @param h 
 */
  virtual void clear(int16_t x, int16_t y, int16_t w, int16_t h);

  virtual void drawText(int16_t x, int16_t y, int16_t h, String &text);

  virtual void drawText(int16_t x, int16_t y, int16_t h, char *text);

  virtual void drawDot(int16_t x, int16_t y, int16_t h, bool fill);
};

#endif // DisplayAdapter_H