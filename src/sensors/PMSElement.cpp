/**
 * @file PMSElement.cpp
 * 
 * @brief Sensor Element for the HomeDing Library to read data from a Plantower PMS5003 sensors and create actions.
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
#include <Board.h>
#include <Element.h>

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
    LOGGER_EERR("do not use `onchange` -> `onvalue`");
    _valueAction = value; // save the actions

  } else if (_stricmp(name, ACTION_ONVALUE) == 0) {
    _valueAction = value; // save the actions

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

  _nextRead = _board->getSeconds() + 4;

  Element::start();

  _pmsSerial = new SoftwareSerial();
  _pmsSerial->begin(9600, SWSERIAL_8N1, _pinrx, _pintx, false, 128);
  _pmsSerial->enableRx(false);
  _datapos = -1; // means input is not open

} // start()


/**
 * @brief Give some processing time to the Element to check for next actions.
 * Example bytes:
 * #42 #4d #00 #1c #00 #06 #00 #0d #00 #12 #00 #06 #00 #0d #00 #12
 * #04 #e3 #01 #73 #00 #5d #00 #18 #00 #08 #00 #00 #98 #00 #03 #65
 */
void PMSElement::loop()
{
  unsigned int now = _board->getSeconds();

  if (_nextRead <= now) {
    // do something

    if (_datapos < 0) {
      // enable receive mode in SoftwareSerial
      _pmsSerial->enableRx(true);
      _datapos = 0;
    } // if

    if (_pmsSerial->overflow()) {
      // just try again in next loop()
      _pmsSerial->enableRx(false);
      _datapos = -1;
      return;
    } // if

    while ((_datapos >= 0) && (_pmsSerial->available())) {
      int b = _pmsSerial->read();

      // collect data to buffer
      if (_datapos == 0) {
        if (b == 0x42) {
          // new data package start with 0x42
          _data[_datapos++] = b;
        }

      } else if (_datapos == 1) {
        if (b == 0x4d) {
          _data[_datapos++] = b; // 0x4d (fixed) starts the data sequence
        } else {
          _datapos = 0; // re-start
        }

      } else if (_datapos < 32) {
        _data[_datapos++] = b; // real data

      } else if (_datapos == 32) {
        // all data received
        _pmsSerial->enableRx(false);

        // check checksum
        int checksum = 0;
        int n;
        for (n = 0; n < 32 - 2; n++)
          checksum += _data[n];

        if (checksum == PWSDATA(14)) {
          // valid data
          // _value = PWSDATA(1); // PM1.0
          _value = PWSDATA(2);
          // _value = PWSDATA(3); // PM10
          LOGGER_ETRACE("value: %d", _value);

          String vs;
          vs.concat(PWSDATA(1)); // PM1.0
          vs.concat(',');
          vs.concat(PWSDATA(2)); // PM2.5
          vs.concat(',');
          vs.concat(PWSDATA(3)); // PM10

          LOGGER_ETRACE("value: %s", vs.c_str());

          if (_valueAction.length() > 0) {
            // gdb_do_break();
            _board->dispatch(_valueAction, _value);
          }
        } else {
          LOGGER_ETRACE("bad checksum.");
        } // if
        _datapos = -1;
        _nextRead = now + _readTime;
      } // if
      yield();
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
