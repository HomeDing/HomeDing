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

/**
 * @brief static factory function to create a new PMSElement
 * @return PMSElement* created element
 */
Element *PMSElement::create()
{
  return (new PMSElement());
} // create()


/* ===== Element functions ===== */

/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool PMSElement::set(const char *name, const char *value)
{
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

  _isOpen = false;
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

    if (!_isOpen) {
      if (!_pmsSerial)
        _pmsSerial = new SoftwareSerial();
      _pmsSerial->begin(9600, SWSERIAL_8N1, _pinrx, _pintx, false, 128);
      _isOpen = true;
      _datapos = 0;
    } // if

    if (_pmsSerial->overflow()) {
      Serial.printf("X");
      _pmsSerial->end();
      // just try again
      _isOpen = false;
      return;
    } // if

    while (_pmsSerial->available()) {
      int b = _pmsSerial->read();

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
          _isOpen = false;
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
          } // if

        } // if
      } // if
    } // while
  } // if
} // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void PMSElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
  callback(PROP_VALUE, String(_value).c_str());
} // pushState()

// End
