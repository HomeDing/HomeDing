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

#ifndef DisplayAdapterSH1106_H
#define DisplayAdapterSH1106_H

#include <SH1106Wire.h>
#include <displays/DisplayAdapterOLED.h>


class DisplayAdapterSH1106 : public DisplayAdapterOLED
{
public:
  /**
   * @brief Construct a new Display Adapter for a SH1106 display
   * using specific parameters.
   */
  DisplayAdapterSH1106(int address, int h)
  {
    _address = address;
    _h = h;
  } // DisplayAdapterSH1106()


  bool init(Board *board)
  {
    if (!disp) {
      // allocate a new class for this display

      if (!WireUtils::exists(_address)) {
        LOGGER_ERR("not found %d", _address);
        return (false);

      } else {
        OLEDDISPLAY_GEOMETRY res = (_h == 64 ? GEOMETRY_128_64 : GEOMETRY_128_32);
        disp = new (std::nothrow) SH1106Wire(_address, board->I2cSda, board->I2cScl, res);
      }
    } // if

    if (!disp) {
      return (false);

    } else {
      disp->init();
      DisplayAdapterOLED::init(board, disp);
    } // if
    return (true);
  }; // init()

private:
  /**
   * @brief I2C Display device address.
   */
  int _address;

  int _h;

  SH1106Wire *disp = nullptr;

}; // class

#endif // DisplayAdapterSH1106_H