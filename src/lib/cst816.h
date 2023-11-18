
#include "Arduino.h"
#include <Wire.h>
#include <WireUtils.h>

#define CTS816_I2C_ADDRESS 0x15

struct CST816point_t {
  uint16_t x;
  uint16_t y;
};


class CST816 {
public:
  CST816(int interruptPin, int resetPin, uint16_t _width, uint16_t _height) {
    _intPin = interruptPin;
    _rstPin = resetPin;
  };

  void begin(TwoWire &w) {
    _wire = &w;

    if (_rstPin >= 0) {
      pinMode(_rstPin, OUTPUT);
      digitalWrite(_rstPin, 0);
      delay(200);
      digitalWrite(_rstPin, 1);
      delay(200);
    }
  };


  bool getTouchPoints(CST816point_t *p) {
    uint8_t raw[6];
    WireUtils::readBuffer(CTS816_I2C_ADDRESS, 0x01, raw, 6);

    // ignore the reported gesture in raw[0]
    if (raw[1]) {
      // Serial.printf("CST816:: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
      //               raw[0], raw[1], raw[2], raw[3], raw[4], raw[5]);
      p->x = ((raw[2] & 0x0F) << 8) + raw[3];
      p->y = ((raw[4] & 0x0F) << 8) + raw[5];
      return (true);
    }

    return (false);
  };

private:
  TwoWire *_wire;
  int _intPin;
  int _rstPin;
  uint16_t width;
  uint16_t height;

  uint8_t _work[2];
};
