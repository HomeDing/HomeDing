/**
 * @file DisplayAdapterMAX7219.h
 *
 * @brief DisplayAdapter implementation for the HomeDing library
 * adapting LCD displays using the HD44780 chip.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * -----
 * * 25.07.2018 created by Matthias Hertel
 * * 07.12.2020 no write text beyond textline end.
 */

#ifndef DISPLAYADAPTERMAX7219_H
#define DISPLAYADAPTERMAX7219_H

#include <displays/DisplayAdapterGFX.h>

#include <SPI.h>

#define REG_DECODEMODE 0x09
#define REG_INTENSITY 0x0A
#define REG_SCANLIMIT 0x0B
#define REG_SHUTDOWN 0x0C
#define REG_DISPLAYTEST 0x0F


class DisplayAdapterMAX7219 : public DisplayAdapterGFX {
public:
  ~DisplayAdapterMAX7219() = default;

  bool start() override {
    // LOGGER_JUSTINFO("init: w:%d, h:%d, r:%d", conf->width, conf->height, conf->rotation);
    // LOGGER_JUSTINFO("  pins: l:%d, r:%d", conf->lightPin, conf->resetPin);
    // LOGGER_JUSTINFO("   i2c: adr:%d, sda:%d, scl:%d", conf->i2cAddress, conf->i2cSDA, conf->i2cSCL);
    // LOGGER_JUSTINFO("   spi: cs:%d, mosi:%d, clk:%d", conf->spiCS, conf->spiMOSI, conf->spiCLK);

    int bytes = conf->width * conf->height;
    _csPin = conf->spiCS;

    if (bytes == 0 || bytes > 10 * 1024) {
      LOGGER_ERR("display dimensions ?");
      return (false);
    }
    display = new (std::nothrow) GFXcanvas1(conf->width, conf->height);

    if (!display) {
      LOGGER_ERR("not found");
      return (false);

    } else {
      gfxDisplay = (Adafruit_GFX *)display;
      backColor565 = 0x0000;
      drawColor565 = 0xFFFF;
      DisplayAdapterGFX::start();

      pinMode(_csPin, OUTPUT);
      digitalWrite(_csPin, HIGH);

      SPI.begin();
      _writeAll(REG_DISPLAYTEST, 0);   // no test mode
      _writeAll(REG_SCANLIMIT, 0x07);  // all digits
      _writeAll(REG_DECODEMODE, 0);    // no decode
      _writeAll(REG_SHUTDOWN, 1);      // enable
      setBrightness(20);
      display->fillScreen(backColor565);
      flush();
    }  // if
    return (true);
  };  // init()


  /**
   * Control the brightness of the whole display.
   * @param brightness = 0...100
   */
  virtual void setBrightness(uint8_t brightness) override {
    int b = (brightness * 16) / 100;

    b = constrain(b, 0, 16);

    if (b == 0) {
      _writeAll(REG_SHUTDOWN, 0);
    } else {
      _writeAll(REG_SHUTDOWN, 1);
      _writeAll(REG_INTENSITY, b - 1);
    }
  }  // setBrightness()


  /**
   * @brief Set the drawing and font color.
   * This is not in use as drawing is always LED-ON.
   * @param col unused.
   */
  virtual void setColor(UNUSED uint32_t col) override{
    // LOGGER_JUSTINFO("no-setColor");
  };


  /**
   * @brief Get the drawing and font color.
   * @return color always white.
   */
  virtual uint32_t getColor() override {
    return (0x00FFFFFF);  // white
  };


  /**
   * @brief send all pixels in the buffer to the attached display.
   */
  virtual void flush() override {
    // LOGGER_JUSTINFO("flush()");

    int lineOffset = ((conf->width + 7) / 8);  ///< offset in buffer to 1 line down.
    uint8_t *buffer = display->getBuffer();

    // print buffer on Serial log
    // for (int y = 0; y < conf->height; y++) {
    //   uint8_t b = 0;
    //   for (int x = 0; x < conf->width; x++) {
    //     if ((x % 8) == 0) {
    //       b = *buffer++;
    //     }
    //     if (b & 0x80) {
    //       Serial.print('*');
    //     } else {
    //       Serial.print('.');
    //     }
    //     b = b << 1;
    //   }
    //   Serial.println();
    // }
    // Serial.println();
    // buffer = display->getBuffer();

    // shift to display !
    SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
    for (int y = 0; y < 8; y++) {
      // transfer all data from one horizontal line into daisychained buffers.
      digitalWrite(_csPin, LOW);
      for (int x = 0; x < 4; x++) {
        uint8_t d = buffer[x + y * lineOffset];
        SPI.transfer(y + 1);
        SPI.transfer(d);
      }  // for
      // save data into display.
      digitalWrite(_csPin, HIGH);
    }  // for
    SPI.endTransaction();
  };

private:
  /**
   * @brief Reference to the used library object
   */
  GFXcanvas1 *display = nullptr;

  uint8_t _csPin;

  /**
   * @brief Write data to all modules of the display
   * @param address
   * @param data
   */
  void _writeAll(byte address, byte data) {
    int modules = (conf->width + 7) / 8;
    digitalWrite(_csPin, LOW);
    while (modules) {
      SPI.transfer(address);
      SPI.transfer(data);
      modules--;
    }
    digitalWrite(_csPin, HIGH);
  }
};

#endif  // DISPLAYADAPTERMAX7219_H