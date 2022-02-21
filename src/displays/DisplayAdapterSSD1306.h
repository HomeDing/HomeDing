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


class DisplayAdapterSSD1306 : public DisplayAdapterOLED {
public:
  /**
   * @brief Construct a new Display Adapter for a SSD1306 display
   * using specific parameters.
   */
  DisplayAdapterSSD1306(int address, int height, int rotation = 0)
    : DisplayAdapterOLED(address, height, rotation) {
  }  // DisplayAdapterSSD1306()

  virtual ~DisplayAdapterSSD1306() = default;


  bool init(Board *board) {
    if (!disp) {
      // allocate a new class for this display

      if (!WireUtils::exists(_address)) {
        LOGGER_ERR("not found %d", _address);
        return (false);

      } else {
        OLEDDISPLAY_GEOMETRY res = (_height == 64 ? GEOMETRY_128_64 : GEOMETRY_128_32);
        disp = new (std::nothrow) SSD1306Wire(_address, board->I2cSda, board->I2cScl, res);
      }
    }  // if

    if (!disp) {
      return (false);

    } else {
      disp->init();
      DisplayAdapterOLED::init(board, disp);
    }  // if
    return (true);
  };  // init()

private:
  SSD1306Wire *disp = nullptr;
};  // class

#endif  // DisplayAdapterSSD1306_H