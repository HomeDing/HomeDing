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

class DisplayAdapterSH1106 : DisplayAdapterOLED
{
public:
  /**
   * @brief Construct a new Display Adapter for a SH1106 display
   * using specific parameters.
   */
  DisplayAdapterSH1106(int address, int h)
  {
    DisplayAdapterOLED(address, h);
    _address = address;
  } // DisplayAdapterSH1106()


  bool init(Board *board)
  {
    // test if a device is attached
    if (!WireUtils::exists(_address)) {
      LOGGER_INFO("not found %d", _address);
      return (false);

    } else {
      // LOGGER_TRACE("setupDisplay...");
      SH1106Wire *d = new SH1106Wire(_address, board->I2cSda, board->I2cScl, _resolution);
      d->init();
      d->connect();

      DisplayAdapterOLED::init(board, d);
    } // if
    return (true);
  }; // init()

}; // class

#endif // DisplayAdapterSH1106_H