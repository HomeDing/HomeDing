/**
 * @file PMSElement.cpp
 * @brief Element Template class.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license.
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:see PMSElement.h
 */


#include <Arduino.h>
#include <Element.h>
#include <Board.h>

#include "PMSElement.h"
#include <ElementRegistry.h>

#include <cstring>

/* ===== Define local constants and often used strings ===== */

/* ===== Define local MACROS ===== */

// get a sensor data from the data buffer
// starting data[4+5] = PWSDATA(1) as counted in reference manual
#define PWSDATA(n) ((_data[2 + 2 * n] << 8) + _data[2 + 2 * n + 1])

/* ===== Static factory function ===== */

// SoftwareSerial pmsSerial2(2, 0, false, 256);
bool isOpen = false;

/**
 * @brief static factory function to create a new PMSElement
 * @return PMSElement* created element
 */
Element *PMSElement::create()
{
  return (new PMSElement());
} // create()


/* ===== Element functions ===== */

// PMSElement::PMSElement() {}

// maybe: overwrite the init() function.
// void PMSElement::init(Board *board)
// {
//   LOGGER_ETRACE("init()");
//   Element::init(board);
//   // do something here like initialization
// } // init()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool PMSElement::set(const char *name, const char *value)
{
  LOGGER_ETRACE("set(%s, %s)", name, value);
  bool ret = true;

  if (_stricmp(name, "pinrx") == 0) {
    _pinrx = _atopin(value);

  } else if (_stricmp(name, "pintx") == 0) {
    _pintx = _atopin(value);

  } else if (_stricmp(name, "readtime") == 0) {
     _readTime = _atotime(value);

  } else if (_stricmp(name, "onChange") == 0) {
    _changeAction = value; // save the actions
    // } else if (_stricmp(name, "doAction") == 0) {
    // make something

  } else {
    ret = Element::set(name, value);
  } // if

  return (ret);
} // set()


/**
 * @brief Activate the PMSElement.
 */
void PMSElement::start()
{
  LOGGER_ETRACE("start(%d, %d)", _pinrx, _pintx);

  // _pmsSerial->begin(9600);
  // _pmsSerial->
  isOpen = false;
  _nextRead = _board->getSeconds() + 4;

  Element::start();
} // start()


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void PMSElement::loop()
{
  unsigned int now = _board->getSeconds();

  if (_nextRead <= now) {
    // do something

    if (!isOpen) {
      if (!_pmsSerial)
        _pmsSerial = new SoftwareSerial(_pinrx, _pintx, false, 128);
      _pmsSerial->begin(9600);
      isOpen = true;
      _datapos = 0;
    } // if

    if (_pmsSerial->overflow()) {
      Serial.printf("X");
      _pmsSerial->end();
      // just try again
      isOpen = false;
      return;
    } // if

    while (_pmsSerial->available()) {
      int b = _pmsSerial->read();

      // raw logging
      // if (b == 0x42)
      //   Serial.printf("\n");
      // Serial.printf("%02x", b);

      // collect data to buffer
      if ((_datapos == 0) && (b == 0x42)) {
        // possibly new data package
        _data[_datapos++] = b; // 0x42 (fixed) starts the data sequence
      } else if ((_datapos == 1) && (b == 0x4d)) {
        _data[_datapos++] = b; // 0x4d (fixed) starts the data sequence
      } else if (_datapos > 1) {
        _data[_datapos++] = b; // data

        if (_datapos == 32) {
          // all data received
          _pmsSerial->end();
          isOpen = false;
          _nextRead = now + _readTime;

          // check checksum
          int checksum = 0;
          int n;
          for (n = 0; n < 32-2; n++) checksum += _data[n];

          if (checksum == PWSDATA(14)) {
            // valid data
            _value = PWSDATA(2);
            _datapos = 0;
            LOGGER_ETRACE("value = %d", _value);

            if (_changeAction.length() > 0)
              _board->dispatch(_changeAction, _value);

            // for (n = 0; n < 32; n++) Serial.printf("%02x", _data[n]);
            // Serial.printf("\n");
          } // if

        } // if
      } // if
    } // while
  }

} // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void PMSElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
  callback("value", String(_value).c_str());
} // pushState()


// maybe: overwrite the term() function,


/* ===== Register the Element ===== */

// As long as the Element is project specific or is a element always used
// the registration is placed here without using a register #define.

// When transferred to the HomeDing library a #define like the
// HOMEDING_INCLUDE_My should be used to allow the sketch to select the
// available Elements. See <HomeDing.h> the move these lines to PMSElement.h:

// #ifdef HOMEDING_REGISTER
// Register the PMSElement onto the ElementRegistry.
bool PMSElement::registered =
    ElementRegistry::registerElement("pms", PMSElement::create);
// #endif

// End
