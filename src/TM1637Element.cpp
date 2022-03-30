/**
 * @file TM1637Element.cpp
 * @brief The P8913Element implements communication with P9813 LED driver chips to implement RGB lights.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD 3-Clause style license,
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:see TM1637Element.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include "TM1637Element.h"


#define TRACE(...) LOGGER_JUSTINFO(__VA_ARGS__)

#define TM1637_MAXDIGITS 10
#define TM1637_DELAY 20

#define TM1637_CMD_WRITE 0x40    // start writing display data
#define TM1637_CMD_ADDRESS 0xC0  // at address 0
#define TM1637_CMD_CONTROL 0x80  // set on/off and brightness
#define TM1637_ON 0x08

#define TYPE_TM1637 1
#define TYPE_TM1638 2

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new TM1637Element
 * @return TM1637Element* created element
 */
Element *TM1637Element::create() {
  return (new TM1637Element());
}  // create()


/* ===== Element functions ===== */

/**
 * @brief Send a sequence of data and clock signals.
 * @param seq Sequence as String.
 * @param data 1 byte data to be send.
 */
int TM1637Element::_io(const char *seq, int data) {
  int ret = 0;

  const char *p = seq;
  while (p && *p) {
    switch (*p++) {
      // Clock Pin
      case 'C': digitalWrite(_clockPin, HIGH); break;
      case 'c': digitalWrite(_clockPin, LOW); break;
      // Data Pin
      case 'D': digitalWrite(_dataPin, HIGH); break;
      case 'd': digitalWrite(_dataPin, LOW); break;

      // 1 bit, not needed
      // case '0': _send("cdC"); break;
      // case '1': _send("cDC"); break;

      // start & stop sequences
      case '(': _io("CDdc"); break;
      case ')': _io("cdCD"); break;

      // read 1 bit
      case 'r':
        _io("cD");
        pinMode(_dataPin, INPUT);
        digitalWrite(_clockPin, HIGH);
        delayMicroseconds(TM1637_DELAY);
        ret = digitalRead(_dataPin);
        pinMode(_dataPin, OUTPUT);
        digitalWrite(_dataPin, LOW);
        delayMicroseconds(TM1637_DELAY);
        break;

      // send data byte Value
      case 'V':
        for (int i = 8; i > 0; i--) {
          if (data & 0x01) {
            _io("cDC");
          } else {
            _io("cdC");
          }
          data >>= 1;
        }
        ret = _io("r");
        break;
    }
    delayMicroseconds(TM1637_DELAY);
  }
  return (ret);
}  // _send()

// display raw data
void TM1637Element::_displayRaw(int *data) {
  _io("(V)", TM1637_CMD_WRITE);
  _io("(V", TM1637_CMD_ADDRESS + 0);
  for (unsigned int i = 0; i < _digits; i++) {
    _io("V", data[i]);
  }
  _io(")(V)", TM1637_CMD_CONTROL + (_brightness > 0) ? TM1637_ON + (_brightness - 1) : 0);
}  // _displayRaw()


// Display a number or a time from a string.
void TM1637Element::_display(const char *s) {
  unsigned int pos = 0;
  int raw[4] = { 0, 0, 0, 0 };
  bool canShift = true;

  while (s && *s && pos < _digits) {
    if (isdigit(*s)) {
      raw[pos++] = _numSegments[*s - '0'];

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

  // shift right if it is just a number
  while ((canShift) && (pos < _digits)) {
    for (int n = 3; n > 0; n--) {
      raw[n] = raw[n - 1];
    }
    raw[0] = 0;
    pos++;
  }
  _displayRaw(raw);
}  // display()


const int TM1637Element::_numSegments[10] = { 0x3f, 0x06, 0x5B, 0x4F, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f };


/* ===== Element functions ===== */

/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool TM1637Element::set(const char *name, const char *value) {
  TRACE("set %s=%s", name, value);
  bool ret = true;

  if (Element::set(name, value)) {
    // done.
  } else if (_stricmp(name, "value") == 0) {
    _value = value;
    _needUpdate = true;

  } else if (_stricmp(name, "brightness") == 0) {
    int b = _atoi(value);
    b = constrain(b, 0, 8);
    _brightness = b;
    _needUpdate = true;

  } else if (_stricmp(name, "type") == 0) {
    if (_stricmp(value, "tm1637")) {
      _type = TYPE_TM1637;
      _ioDelay = TM1637_DELAY;
    // } else if (_stricmp(value, "tm1638")) {
    //   _type = TYPE_TM1638;
    //   _ioDelay = TM1637_DELAY;
    }

  } else if (_stricmp(name, "datapin") == 0) {
    _dataPin = _atopin(value);

  } else if (_stricmp(name, "clockpin") == 0) {
    _clockPin = _atopin(value);

  } else if (_stricmp(name, "cspin") == 0) {
    _csPin = _atopin(value);

  } else {
    ret = false;
  }  // if
  return (ret);
}  // set()


/**
 * @brief Activate the TM1637Element.
 */
void TM1637Element::start() {
  TRACE("start()");

  if ((_type == TYPE_TM1637) && (_dataPin >= 0) && (_clockPin >= 0)) {
    _digits = 4;
    // _digits = constrain(_digits, 0, TM1637_MAXDIGITS);

    pinMode(_clockPin, OUTPUT);
    pinMode(_dataPin, OUTPUT);
    digitalWrite(_clockPin, HIGH);
    digitalWrite(_dataPin, HIGH);

    Element::start();
    _needUpdate = true;

  // } else if ((_type == TYPE_TM1638) && (_dataPin >= 0) && (_clockPin >= 0) && (_csPin >= 0)) {
  //   _digits = 8;

  //   pinMode(_clockPin, OUTPUT);
  //   pinMode(_dataPin, OUTPUT);
  //   pinMode(_csPin, OUTPUT);
  //   digitalWrite(_clockPin, HIGH);
  //   digitalWrite(_dataPin, HIGH);

  }
}  // start()


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void TM1637Element::loop() {
  // do something
  if (_needUpdate) {
    TRACE("update %d %s", _brightness, _value);
    _display(_value.c_str());
    _needUpdate = false;
  }
}  // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void TM1637Element::pushState(
  std::function<void(const char *pName, const char *eValue)> callback) {
  Element::pushState(callback);
  callback(PROP_VALUE, _value.c_str());
}  // pushState()


// End
