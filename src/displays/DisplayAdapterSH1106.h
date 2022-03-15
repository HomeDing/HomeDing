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


class DisplayAdapterSH1106 : public DisplayAdapterOLED {
public:
  bool start() override {
    if (!disp) {
      // allocate a new class for this display

      if (!WireUtils::exists(conf->i2cAddress)) {
        LOGGER_ERR("not found %d", conf->i2cAddress);
        return (false);

      } else {
        OLEDDISPLAY_GEOMETRY res = (conf->height == 64 ? GEOMETRY_128_64 : GEOMETRY_128_32);
        disp = new (std::nothrow) SH1106Wire(conf->i2cAddress, conf->i2cSDA, conf->i2cSCL, res);
      }
    }  // if

    if (!disp) {
      return (false);

    } else {
      display = disp;
      DisplayAdapterOLED::start();
    }  // if
    return (true);
  };  // init()

private:
  SH1106Wire *disp = nullptr;

};  // class

#endif  // DisplayAdapterSH1106_H