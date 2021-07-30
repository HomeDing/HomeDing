// @file: tm1637.h
// class for driving the simple 4 digit displays using a tm1637.
// The modules that work with this implementation have 4 digits and a colon between the 2. and 3. digit
// that is controlled by the uppermost bit in data[1].
// The decimal points cannot be controlled.

// This class can display time strings and integers in range -999 to 9999.

#pragma once

#include <Arduino.h>

#define TM1637_MAXDIGITS 6
#define TM1637_DELAY 20

#define TM1637_CMD_WRITE 0x40 // start writing display data
#define TM1637_CMD_ADDRESS 0xC0 // at address 0
#define TM1637_CMD_CONTROL 0x80 // set on/off and brightness
#define TM1637_ON 0x08

class TM1637 {
public:
  TM1637(int clockPin, int dataPin, int digits = 4) {
    _clockPin = clockPin;
    _dataPin = dataPin;
    _digits = constrain(digits, 0, TM1637_MAXDIGITS);
    _on = TM1637_ON;
    _brightness = 2;

    pinMode(_clockPin, OUTPUT);
    pinMode(_dataPin, OUTPUT);
    digitalWrite(_clockPin, HIGH);
    digitalWrite(_dataPin, HIGH);
  } // TM1637()


  // return current display brightness
  int getBrightness() {
    return (_brightness);
  } // setBrightness()


  // set current display brightness
  void setBrightness(int b) {
    b = constrain(b, 0, 8);
    if (b != _brightness + 1) {
      _on = (b > 0) ? TM1637_ON : 0;
      _brightness = b - 1;
      display(_data);
    }
  } // setBrightness()


  // display raw data
  void display(int *data) {
    sendStart();
    sendByte(TM1637_CMD_WRITE);
    sendStop();

    sendStart();
    sendByte(TM1637_CMD_ADDRESS + 0);
    for (unsigned int i = 0; i < _digits; i++) {
      _data[i] = data[i];
      sendByte(data[i]);
    }
    sendStop();

    sendStart();
    sendByte(TM1637_CMD_CONTROL + _on + _brightness);
    sendStop();
  } // display()


  // Display a number or a time from a string.
  void display(const char *s) {
    unsigned int pos = 0;
    int raw[4] = {0, 0, 0, 0};
    bool canShift = true;

    while (s && *s && pos < _digits) {
      if (isdigit(*s)) {
        raw[pos++] = numSegments[*s - '0'];

      } else if (*s == '-') {
        raw[pos++] = 0x40;

      } else if (*s == ':') {
        // The ':' is always in 0x80 in digit offset 1
        raw[1] += 0x80;
        pos = 2;
        canShift = false;
      }
      s++;
    }

    // shift right is just a number
    while ((canShift) && (pos < _digits)) {
      for (int n = 3; n > 0; n--) {
        raw[n] = raw[n - 1];
      }
      raw[0] = 0;
      pos++;
    }
    display(raw);
  } // display()


  // Display a number or a time from a string.
  void display(String s) {
    display(s.c_str());
  } // display

private:
  // segments for numbers 0..9
  static const int numSegments[10];

  /// The pin with the clock signal.
  int _clockPin;

  /// The pin with the data signal.
  int _dataPin;

  int _on = TM1637_ON;

  /// number of digits the display supports
  unsigned int _digits;

  /// current brightness level
  int _brightness;

  // current data to be displayed.
  int _data[TM1637_MAXDIGITS];

  // send a start sequence.
  void sendStart() {
    digitalWrite(_clockPin, HIGH);
    digitalWrite(_dataPin, HIGH);
    digitalWrite(_dataPin, LOW);
    digitalWrite(_clockPin, LOW);
  }

  // send a stop sequence.
  void sendStop() {
    digitalWrite(_clockPin, LOW);
    digitalWrite(_dataPin, LOW);
    digitalWrite(_clockPin, HIGH);
    digitalWrite(_dataPin, HIGH);
  }

  // read a bit like ACK.
  int readBit() {
    digitalWrite(_clockPin, LOW);
    digitalWrite(_dataPin, HIGH);
    pinMode(_dataPin, INPUT);
    digitalWrite(_clockPin, HIGH);

    delayMicroseconds(TM1637_DELAY);
    int ret = digitalRead(_dataPin);

    pinMode(_dataPin, OUTPUT);
    digitalWrite(_dataPin, LOW);
    delayMicroseconds(TM1637_DELAY);

    return (ret);
  }

  void sendByte(int data) {
    // shift out data 8 bits LSB first
    for (uint8_t i = 8; i > 0; i--) {
      digitalWrite(_clockPin, LOW);
      digitalWrite(_dataPin, data & 0x01);
      digitalWrite(_clockPin, HIGH);
      data >>= 1;
    }
    // get ACKNOWLEDGE
    readBit();
  } // sendByte
};

