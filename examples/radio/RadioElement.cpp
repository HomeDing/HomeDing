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

#include <RDSParser.h>
#include <Wire.h>
#include <radio.h>
#include <si4703.h>
#include <si47xx.h>
#include <RDA5807M.h>

/* ===== Define local constants and often used strings ===== */

#define TRACE(...) // LOGGER_ETRACE(__VA_ARGS__)


class RadioElementImpl {
public:
  RADIO_FREQ radioFreq = 0;
};


#define FIX_BAND RADIO_BAND_FM  ///< The band that will be tuned by this sketch is FM.

// The radio library only supports one radio per device so all references and data can be static.
// That simplifies the callback rds functions.

// SI4703 radio;
// SI47xx radio;
RDA5807M radio;

RDSParser rds;

bool _newSN = false;
bool _newR = false;
String _stationName;
String _rdsText;
RADIO_INFO _ri;

// candidates for class
unsigned long _nextInfoCheck = 0;
unsigned long _nextRDSCheck = 0;

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
}  // create()


/* ===== Element functions ===== */

/**
 * @brief Construct a new RadioElement
 */
RadioElement::RadioElement() {
  _impl = new RadioElementImpl();
  // startupMode = Element::STARTUPMODE::System;
}


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool RadioElement::set(const char *name, const char *value) {
  TRACE("set %s=%s", name, value);

  bool ret = true;

  if (Element::set(name, value)) {
    // ok.

  } else if (_stricmp(name, "volume") == 0) {
    int v = _atoi(value);
    if (active && v != _volume) {
      radio.setVolume(v);
      radio.setMute(_mute || (v == 0));
    }
    _volume = v;

  } else if (_stricmp(name, "frequency") == 0) {
    int v = _atoi(value);
    if (active && v != _impl->radioFreq) {
      radio.setFrequency(v);
      HomeDing::Actions::push(_frequencyAction, v);
    }
    _impl->radioFreq = v;

  } else if (_stricmp(name, "mono") == 0) {
    _mono = _atob(value);
    if (active) {
      radio.setMono(_mono);
    }

  } else if (_stricmp(name, "seek") == 0) {
    if (active) {
      if (_atob(value))
        radio.seekUp(true);
      else
        radio.seekDown(true);
    }

  } else if (_stricmp(name, "mute") == 0) {
    _mute = _atob(value);
    if (active) {
      radio.setMute(_mute || (_volume == 0));
    }

  } else if (_stricmp(name, "softmute") == 0) {
    _softMute = _atob(value);
    if (active) {
      radio.setSoftMute(_softMute);
    }

  } else if (_stricmp(name, "bassboost") == 0) {
    _bassBoost = _atob(value);
    if (active) {
      radio.setBassBoost(_bassBoost);
    }

    // setup properties

  } else if (_stricmp(name, "onStationName") == 0) {
    _stationAction = value;

  } else if (_stricmp(name, "onRDSText") == 0) {
    _rdsTextAction = value;

  } else if (_stricmp(name, "onFrequency") == 0) {
    _frequencyAction = value;

  } else if (name == HomeDing::Actions::Address) {
    _address = _atoi(value);

  } else if (_stricmp(name, "antenna") == 0) {
    _antenna = _atoi(value);

  } else if (_stricmp(name, "resetpin") == 0) {
    _resetpin = _atopin(value);

  } else {
    ret = false;
  }  // if

  return (ret);
}  // set()

// radio

/**
 * @brief Activate the RadioElement.
 */
void RadioElement::start() {
  TRACE("start()");

  // Verify parameters
  // __radioelement = this;

  // Enable information to the Serial port
  radio.debugEnable(true);
  radio._wireDebug(loglevel == 2);  // debug the wire protocol on loglevel 2
  _found = radio.initWire(Wire);

  if (!_found) {
    LOGGER_EERR("not found");

  } else {
    // Initialize the Radio
    radio.setup(RADIO_RESETPIN, _resetpin);
    radio.setup(RADIO_MODEPIN, _board->I2cSda);  // SI4703 requires this, others ignore.
    radio.setup(RADIO_I2CADDRESS, 0);           //  use default or check some addresses
    if (_antenna) {
      radio.setup(RADIO_ANTENNA, _antenna);
    }

    // Set all radio setting to the fixed values.
    radio.setBandFrequency(FIX_BAND, _impl->radioFreq);
    radio.setVolume(_volume);
    radio.setMono(_mono);
    radio.setMute(_mute || (_volume == 0));

    Element::start();
    HomeDing::Actions::push(_frequencyAction, _impl->radioFreq);

    TRACE("SETUP RDS...");
    // setup the information chain for RDS data.
    radio.attachReceiveRDS(RDS_process);

    rds.attachServiceNameCallback([](const char *value) {
      LOGGER_RAW("STATION: %s", value);
      _stationName = value;
      _newSN = true;
    });

    rds.attachTextCallback([](const char *value) {
      LOGGER_RAW("RDSTEXT: %s", value);
      _rdsText = value;
      _newR = true;
    });
  }  // if

  // radio._wireDebug(false);

}  // start()


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void RadioElement::loop() {
  // do something
  unsigned long now = millis();

  if (_found) {
    if (now > _nextInfoCheck) {
      RADIO_INFO newri;
      radio.getRadioInfo(&newri);
      memcpy(&_ri, &newri, sizeof(RADIO_INFO));

      RADIO_FREQ f = radio.getFrequency();
      if (f != _impl->radioFreq) { HomeDing::Actions::push(_frequencyAction, f); }
      _impl->radioFreq = f;
      _nextInfoCheck = now + _checkInfo;
    }

    if (now > _nextRDSCheck) {
      radio.checkRDS();
      _nextRDSCheck = now + _checkRDS;
    }

    if (_newSN) {
      HomeDing::Actions::push(_stationAction, _stationName);
      _newSN = false;
    }

    if (_newR) {
      HomeDing::Actions::push(_rdsTextAction, _rdsText);
      _newR = false;
    }
  }
}  // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void RadioElement::pushState(
  std::function<void(const char *pName, const char *eValue)> callback) {
  Element::pushState(callback);
  callback("frequency", _printInteger(_impl->radioFreq));
  callback("volume", _printInteger(_volume));
  callback("rssi", _printInteger(_ri.rssi));
  callback("snr", _printInteger(_ri.snr));
  callback("mono", _printBoolean(_ri.mono));
  callback("stationname", _stationName.c_str());
  callback("rdstext", _rdsText.c_str());
}  // pushState()


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
