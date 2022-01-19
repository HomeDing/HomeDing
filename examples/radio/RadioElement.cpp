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
#include <HomeDing.h>

#include "RadioElement.h"
#include <ElementRegistry.h>

#include <RDSParser.h>
#include <Wire.h>
#include <radio.h>
#include <si4703.h>
#include <si47xx.h>
// #include <RDA5807M.h>

/* ===== Define local constants and often used strings ===== */

#define FIX_BAND RADIO_BAND_FM ///< The band that will be tuned by this sketch is FM.
#define FIX_STATION 8930 ///< The station that will be tuned by this sketch is 89.30 MHz.
#define FIX_VOLUME 4 ///< The volume that will be set by this sketch is level 4.

// The radio library only supports one radio device so all references and data can be static.
// That simplifies the callback rds functions.

// SI4703 radio;
SI47xx radio;

RDSParser rds;

bool _newSN = false;
bool _newR = false;
String _stationName;
String _rdsText;
RADIO_INFO _ri;

// candidates for class
unsigned int _nextCheck = 0;


// const RadioElement *__radioelement;

void RDS_process(uint16_t block1, uint16_t block2, uint16_t block3, uint16_t block4) {
  // LOGGER_RAW("RDS %d %d %d %d", block1, block2, block3,  block4);
  rds.processData(block1, block2, block3, block4);
}


/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new RadioElement
 * @return RadioElement* created element
 */
Element *RadioElement::create() {
  return (new RadioElement());
} // create()


/* ===== Element functions ===== */

RadioElement::RadioElement() {
  startupMode = Element_StartupMode::System;
}

/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool RadioElement::set(const char *name, const char *value) {
  bool ret = true;
  int v;

  if (_stricmp(name, "volume") == 0) {
    v = _atoi(value);
    if (active && v != _volume) {
      _volume = v;
      radio.setVolume(v);
      radio.setMute(_mute || (v == 0));
      _board->dispatch(_volumeAction, v);
    }

  } else if (_stricmp(name, "frequency") == 0) {
    v = _atoi(value);
    if (active && v != _freq) {
      radio.setFrequency(v);
      _board->dispatch(_frequencyAction, v);
    }
    _freq = _atoi(value);

  } else if (_stricmp(name, "mono") == 0) {
    if (active) {
      radio.setMono(_atob(value));
    }

  } else if (_stricmp(name, "mute") == 0) {
    if (active) {
      _mute = _atob(value);
      radio.setMute(_mute || (_volume == 0));
    }

  } else if (_stricmp(name, "softmute") == 0) {
    if (active) {
      radio.setSoftMute(_atob(value));
    }

  } else if (_stricmp(name, "bassboost") == 0) {
    if (active) {
      radio.setBassBoost(_atob(value));
    }

  } else if (_stricmp(name, "antenna") == 0) {
    _antenna = _atoi(value);
    // set in start() only

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

  } else if (_stricmp(name, "address") == 0) {
    // _resetpin = _atopin(value);

  } else if (_stricmp(name, "resetpin") == 0) {
    _resetpin = _atopin(value);

  } else {
    ret = Element::set(name, value);
  } // if

  return (ret);
} // set()

// radio

/**
 * @brief Activate the RadioElement.
 */
void RadioElement::start() {
  LOGGER_ETRACE("start()");

  // Verify parameters
  // __radioelement = this;

  // Enable information to the Serial port
  radio.debugEnable(true);
  radio._wireDebug(loglevel == 2); // debug the wire protocol on loglevel 2
  _found = radio.initWire(Wire);

  if (!_found) {
    LOGGER_EERR("not found");
  } else {
    // Initialize the Radio
    radio.setup(RADIO_RESETPIN, _resetpin);
    radio.setup(RADIO_SDAPIN, _board->I2cSda); // SI4703 requires this, others ignore.
    radio.setup(RADIO_I2CADDRESS, 0); //  use default or check some addresses
    if (_antenna) {
      radio.setup(RADIO_ANTENNA, _antenna);
    }

    // Set all radio setting to the fixed values.
    radio.setBandFrequency(FIX_BAND, FIX_STATION);
    radio.setVolume(_volume);
    radio.setMono(false);
    radio.setMute(_mute || (_volume == 0));

    // if (parameters ok) {
    Element::start();

    LOGGER_RAW("SETUP RDS...");

    // setup the information chain for RDS data.
    radio.attachReceiveRDS(RDS_process);

    rds.attachServicenNameCallback([](const char *value) {
      LOGGER_RAW("STATION: %s", value);
      _stationName = value;
      _newSN = true;
    });

    rds.attachTextCallback([](const char *value) {
      LOGGER_RAW("RDSTEXT: %s", value);
      _rdsText = value;
      _newR = true;
    });
  } // if

  // radio._wireDebug(false);

} // start()


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void RadioElement::loop() {
  // do something
  if (_found) {
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
  }
} // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void RadioElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback) {
  Element::pushState(callback);
  callback("frequency", _printInteger(_freq));
  callback("volume", _printInteger(_volume));
  callback("rssi", String(_ri.rssi).c_str());
  callback("snr", String(_ri.snr).c_str());
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
