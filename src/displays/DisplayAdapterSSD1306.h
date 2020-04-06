/**
 * @file DisplayAdapterSSD1306.h
 *
 * @brief DisplayAdapter implementation for the HomeDing library
 * adapting OLED displays using the SSD1306 chip.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * -----
 * * 25.07.2018 created by Matthias Hertel
 */

#ifndef DisplayAdapterSSD1306_H
#define DisplayAdapterSSD1306_H

#include <SSD1306Wire.h>
#include <displays/DisplayAdapterOLED.h>

class DisplayAdapterSSD1306 : DisplayAdapterOLED
{
public:
  /**
   * @brief Construct a new Display Adapter for a SH1106 display
   * using specific parameters.
   */
  DisplayAdapterSSD1306(int address, int h)
  {
    _address = address;
    _h = h;
  } // DisplayAdapterSSD1306()


  bool init(Board *board)
  {
    // test if a device is attached
    if (!WireUtils::exists(_address)) {
      LOGGER_ERR("not found %d", _address);
      return (false);

    } else {
      OLEDDISPLAY_GEOMETRY res = (_h == 64 ? GEOMETRY_128_64 : GEOMETRY_128_32);

      // LOGGER_TRACE("setupDisplay...");
      SSD1306Wire *d = new SSD1306Wire(_address, board->I2cSda, board->I2cScl, res);
      d->init();
      d->connect();

      DisplayAdapterOLED::init(board, d);
    } // if
    return (true);
  }; // init()

private:
  /**
   * @brief I2C Display device address.
   */
  int _address;

  int _h;

}; // class

#endif // DisplayAdapterSSD1306_H