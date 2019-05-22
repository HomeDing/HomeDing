/**
 * @file RadioElement.cpp
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
 * Changelog:see RadioElement.h
 */

#include <Arduino.h>
#include <Board.h>
#include <Element.h>

#include "RadioElement.h"
#include <ElementRegistry.h>

#include <RDSParser.h>
#include <Wire.h>
#include <radio.h>
// #include <si4703.h>
#include <RDA5807M.h>

/* ===== Define local constants and often used strings ===== */

#define FIX_BAND RADIO_BAND_FM ///< The band that will be tuned by this sketch is FM.
#define FIX_STATION 8930 ///< The station that will be tuned by this sketch is 89.30 MHz.
#define FIX_VOLUME 4 ///< The volume that will be set by this sketch is level 4.

// The radio library only supports one radio device so all references and data can be static.
// That simplifies the callback rds functions.

// SI4703 radio(D8);
RDA5807M radio;

RDSParser rds;

bool _newSN = false;
bool _newR = false;
String _stationName;
String _rdsText;
RADIO_INFO _ri;

// candidates for class
unsigned int _nextCheck = 0;


// const RadioElement *__radioelement;

void RDS_process(uint16_t block1, uint16_t block2, uint16_t block3, uint16_t block4)
{
  // LOGGER_RAW("RDS %d %d %d %d", block1, block2, block3,  block4);
  rds.processData(block1, block2, block3, block4);
}


// http://lcddevice/$board/radio/r?volume=1
// http://lcddevice/$board/radio/r?volume=12
// http://lcddevice/$board/board/0?loglevel=1

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new RadioElement
 * @return RadioElement* created element
 */
Element *RadioElement::create()
{
  return (new RadioElement());
} // create()


/* ===== Element functions ===== */

RadioElement::RadioElement()
{
  startupMode = Element_StartupMode::System;
}


// maybe: overwrite the init() function.

// void RadioElement::init(Board *board)
// {
//   LOGGER_ETRACE("init()");
//   Element::init(board);
//   // do something here like initialization
// } // init()

// int _volume = FIX_VOLUME;
// int _freq = FIX_STATION;


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool RadioElement::set(const char *name, const char *value)
{
  LOGGER_ETRACE("set(%s, %s)", name, value);
  bool ret = true;
  int v;

  if (_stricmp(name, "volume") == 0) {
    v = _atoi(value);
    if (active && v != _volume) {
      radio.setVolume(v);
      _board->dispatch(_volumeAction, v);
    }
    _volume = v;

  } else if (_stricmp(name, "frequency") == 0) {
    v = _atoi(value);
    if (active && v != _freq) {
      radio.setFrequency(v);
      _board->dispatch(_frequencyAction, v);
    }
    _freq = _atoi(value);

  } else if (_stricmp(name, "onStationName") == 0) {
    _stationAction = value;

  } else if (_stricmp(name, "onRDSText") == 0) {
    _rdsTextAction = value;

  } else if (_stricmp(name, "onFrequency") == 0) {
    _frequencyAction = value;

  } else if (_stricmp(name, "onVolume") == 0) {
    _volumeAction = value;

  } else if (_stricmp(name, "onRSSI") == 0) {
    _rssiAction = value;

    // } else if (_stricmp(name, "doAction") == 0) {
    // make something

  } else {
    ret = Element::set(name, value);
  } // if

  return (ret);
} // set()

// radio

/**
 * @brief Activate the RadioElement.
 */
void RadioElement::start()
{
  LOGGER_ETRACE("start(%d, %d)", SDA, SCL);

  // Verify parameters
  // __radioelement = this;

  // Enable information to the Serial port
  radio.debugEnable(false);

  // Initialize the Radio
  radio.init();

  // Set all radio setting to the fixed values.
  radio.setBandFrequency(FIX_BAND, FIX_STATION);
  radio.setVolume(_volume);
  radio.setMono(false);
  radio.setMute(false);

  // if (parameters ok) {
  Element::start();

  LOGGER_RAW("SETUP RDS...");

  // setup the information chain for RDS data.
  radio.attachReceiveRDS(RDS_process);

  // _scope = this;

  rds.attachServicenNameCallback([](char *value) {
    LOGGER_RAW("STATION: %s", value);
    _stationName = value;
    _newSN = true;
  });

  rds.attachTextCallback([](char *value) {
    LOGGER_RAW("RDSTEXT: %s", value);
    _rdsText = value;
    _newR = true;
  });

  // } // if

} // start()


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void RadioElement::loop()
{
  // do something
  radio.checkRDS();
  unsigned int now = _board->getSeconds();
  RADIO_INFO newri;

  if (_newSN) {
    _board->dispatch(_stationAction, _stationName);
    _newSN = false;
  }

  if (_newR) {
    _board->dispatch(_rdsTextAction, _rdsText);
    _newR = false;
  }

  if (now > _nextCheck) {
    radio.getRadioInfo(&newri);
    if (newri.rssi != _ri.rssi)
      _board->dispatch(_rssiAction, newri.rssi);
    memcpy(&_ri, &newri, sizeof(RADIO_INFO));
    _nextCheck = now + _checkInfo;
  }

} // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void RadioElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
  callback("frequency", String(_freq).c_str());
  callback("volume", String(_volume).c_str());
  callback("stationname", _stationName.c_str());
  callback("rdstext", _rdsText.c_str());
} // pushState()


/* ===== Register the Element ===== */

// As long as the Element is project specific or is a element always used
// the registration is placed here without using a register #define.

// When transferred to the HomeDing library a #define like the
// HOMEDING_INCLUDE_My should be used to allow the sketch to select the
// available Elements. See <HomeDing.h> the move these lines to RadioElement.h:

// #ifdef HOMEDING_REGISTER
// Register the RadioElement onto the ElementRegistry.
bool RadioElement::registered =
    ElementRegistry::registerElement("radio", RadioElement::create);
// #endif

// End
