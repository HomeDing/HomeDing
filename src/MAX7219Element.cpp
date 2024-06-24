/**
 * @file MAX7219Element.cpp
 * @brief Element for driving a MAX7219 chip using SPI.
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
 * Changelog:see MAX7219Element.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <MAX7219Element.h>

#include <SPI.h>

#define TRACE(...)  // LOGGER_ETRACE(__VA_ARGS__)

/* ===== Define local constants and often used strings ===== */

#define REG_DECODEMODE 0x09
#define REG_INTENSITY 0x0A
#define REG_SCANLIMIT 0x0B
#define REG_SHUTDOWN 0x0C
#define REG_DISPLAYTEST 0x0F


/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new MAX7219Element
 * @return MAX7219Element* created element
 */
Element *MAX7219Element::create() {
  return (new MAX7219Element());
}  // create()


/* ===== Element functions ===== */

void MAX7219Element::_write(byte address, byte data) {
  digitalWrite(_csPin, LOW);
  SPI.transfer(address);
  SPI.transfer(data);
  digitalWrite(_csPin, HIGH);
}

void MAX7219Element::_clear() {
  if (_decodeMode != 0x00) {
    _decodeMode = 0x00;  // no decode
    _write(REG_DECODEMODE, _decodeMode);
  }
  for (int i = 1; i <= 8; i++) {
    _write(i, 0b00000000);
  }
}  // _clear()


void MAX7219Element::_setBrightness() {
  if (active) {
    if (_brightness == 0) {
      _write(REG_SHUTDOWN, 0);

    } else {
      _write(REG_SHUTDOWN, 1);
      _write(REG_INTENSITY, _brightness - 1);
    }
  }
}  // _setBrightness()


void MAX7219Element::_writeRow(byte row, byte data) {
  if (_decodeMode != 0x00) {
    _decodeMode = 0x00;  // no decode
    _write(REG_DECODEMODE, _decodeMode);
  }
  _write(row + 1, data);
}

void MAX7219Element::_writeNumber(String number) {
  int len = number.length();
  byte data[8];
  int digits = 0;

  int m = 0;

  memset(data, 0, sizeof(data));

  // TRACE("NUM: %d", number.c_str());

  // fill data array with digits and decimal point

  for (int i = len - 1; i >= 0; i--) {
    char ch = number[i];

    if ((ch == '.') || (ch == ',')) {
      data[digits] = 0x80;  //  set decimal point

    } else if (isdigit(ch)) {
      data[digits] += (ch - '0');
      m |= 1 << digits;  // set this digit in decode mode
      digits++;
    }
  }  // for

  if (_decodeMode != m) {
    _decodeMode = m;  // no decode
    _write(REG_DECODEMODE, m);
  }

  // send all data
  for (int row = 0; row < 8; row++) {
    _write(row + 1, data[row]);
  }
}  // _writeNumber()


/**
 * write a bit pattern
 * @param value in format "0x0f1ef000e0e00000"
 */
void MAX7219Element::_writeM8X8(String value) {
  TRACE("writeM8X8(%s)", value.c_str());

  int row = 1;

  while (value.length() > 1) {
    // take fist 2 chars and take as hex for row
    String x = value.substring(0, 2);
    value.remove(0, 2);

    if (x.equalsIgnoreCase("0x"))
      continue;  // ignore this.

    byte bits = strtol(x.c_str(), nullptr, 16);
    _write(row++, bits);
  }  // while
}  // _writeM8X8()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool MAX7219Element::set(const char *name, const char *value) {
  bool ret = true;

   if (Element::set(name, value)) {
     // ok.

  } else if (name == HomeDing::Action::Value) {
    _value = value;

  } else if (_stricmp(name, "clear") == 0) {
    if (active)
      _clear();

  } else if (_stricmp(name, "brightness") == 0) {
    int b = _atoi(value);
    _brightness = constrain(b, 0, 16);
    _setBrightness();

  } else if (_stricmp(name, "cspin") == 0) {
    _csPin = _atopin(value);

  } else if (_stricmp(name, "mode") == 0) {
    if (_stricmp(value, "numeric") == 0) {
      _mode = Mode::numeric;
    } else if (_stricmp(value, "8x8") == 0) {
      _mode = Mode::m8x8;
    }

  } else {
    ret = false;
  }  // if

  return (ret);
}  // set()


/**
 * @brief Activate the MAX7219Element.
 */
void MAX7219Element::start() {
  TRACE("start()");
  TRACE("csPin=%d", _csPin);
  TRACE("mode=%d", _mode);

  // Verify parameters
  if ((_csPin >= 0) && (_mode != Mode::none)) {
    Element::start();

    SPI.begin();
    pinMode(_csPin, OUTPUT);
    digitalWrite(_csPin, HIGH);
    _write(REG_DISPLAYTEST, 0);           // no test mode
    _write(REG_SCANLIMIT, 0x07);          // all digits

     _clear();
    _setBrightness();
    _lastValue = "";
  }  // if


}  // start()


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void MAX7219Element::loop() {
  // do something
  if (!_value.equals(_lastValue)) {
    if (_mode == Mode::numeric) {
      _writeNumber(_value);
    } else if (_mode == Mode::m8x8) {
      _writeM8X8(_value);
    }
    _lastValue = _value;
  }
}  // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void MAX7219Element::pushState(
  std::function<void(const char *pName, const char *eValue)> callback) {
  Element::pushState(callback);
  callback("mode", _mode == Mode::numeric ? "numeric" : "8x8");
  callback("brightness", _printInteger(_brightness));
  callback("value", String(_value).c_str());
}  // pushState()


// End
