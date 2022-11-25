/**
 * @file AudioElement.cpp
 *
 * @brief The AudioElement supports Audio streaming and audio processing in a background task
 * used to build internet audio streaming devices.
 * It requires 2 ESP32 or ESP32-S3 processor that supports multiple tasks
 * and PSRAM for buffering audio data.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:see AudioElement.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include "AudioElement.h"

#include <Audio.h>

#define TRACE(...)  // LOGGER_ETRACE(__VA_ARGS__)

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new AudioElement
 * @return AudioElement* created element
 */
Element *AudioElement::create() {
  return (new AudioElement());
}  // create()

// codes for sending commands into the audio task

#define AUDIOCMD_NONE 0
#define AUDIOCMD_URL 1
#define AUDIOCMD_VOLUME 2
#define AUDIOCMD_BALANCE 3
#define AUDIOCMD_TONE 4

// These references are set in static variables to make it available in the background audio task.

AudioElement *__element = nullptr;
Board *__board = nullptr;
Audio *__audio = nullptr;
uint32_t __cmd = AUDIOCMD_NONE;


// ===== Background task for audio processing ======

void audioTask(void *parameter) {
  uint32_t cmd;

  while (true) {

    if (__cmd != AUDIOCMD_NONE) {
      cmd = __cmd;
      __cmd = AUDIOCMD_NONE;

      log_n(">> cmd %d\n", cmd);
      if (cmd == AUDIOCMD_URL) {
        __audio->stopSong();
        __board->dispatch(__element->_onStation, "");
        __board->dispatch(__element->_onTitle, "");
        if (!__element->_url.isEmpty()) {
          log_n(".1 %s\n", __element->_url.c_str());
          __audio->connecttohost(__element->_url.c_str());
        }

      } else if (cmd == AUDIOCMD_VOLUME) {
        __audio->setVolume(__element->_volume);

      } else if (cmd == AUDIOCMD_BALANCE) {
        __audio->setBalance(__element->_balance);

      } else if (cmd == AUDIOCMD_TONE) {
        __audio->setTone(__element->_low, __element->_mid, __element->_high);

      }  // if
      cmd = AUDIOCMD_NONE;
    }  // if

    if (__audio->isRunning()) {
      __audio->loop();
    } else {
      sleep(1);
    }
  }
}

void audio_info(const char *info) {
  Logger::LoggerEPrint(__element, LOGGER_LEVEL_TRACE, info);
}


void audio_showstation(const char *value) {
  __board->dispatch(__element->_onStation, value);
}

void audio_showstreamtitle(const char *value) {
  __board->dispatch(__element->_onTitle, value);
}

/* ===== Element functions ===== */

void AudioElement::init(Board *board) {
  TRACE("init()");
  Element::init(board);
  // do something here like initialization

  __board = board;
  __element = this;
  _i2s_bclk = -1;
  _i2s_lrc = -1;
  _i2s_dout = -1;

  // some default values
  _volume = 4;
  _balance = 0;
  _low = 0;
  _mid = 0;
  _high = 0;
}  // init()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool AudioElement::set(const char *name, const char *value) {
  TRACE("set(%s, %s)", name, value);
  bool ret = true;

  if (Element::set(name, value)) {
    // done.

  } else if (_stricmp(name, "url") == 0) {
    _url = value;
    __cmd = AUDIOCMD_URL;

  } else if (_stricmp(name, "volume") == 0) {
    int v = _atoi(value);
    _volume = constrain(v, 0, 21);
    __cmd = AUDIOCMD_VOLUME;

  } else if (_stricmp(name, "balance") == 0) {
    int v = _atoi(value);
    _balance = constrain(v, -16, 16);
    __cmd = AUDIOCMD_BALANCE;

  } else if (_stricmp(name, "low") == 0) {
    int v = _atoi(value);
    _low = constrain(v, -20, 6);
    __cmd = AUDIOCMD_TONE;

  } else if (_stricmp(name, "mid") == 0) {
    int v = _atoi(value);
    _mid = constrain(v, -20, 6);
    __cmd = AUDIOCMD_TONE;

  } else if (_stricmp(name, "high") == 0) {
    int v = _atoi(value);
    _high = constrain(v, -20, 6);
    __cmd = AUDIOCMD_TONE;

    // === Settings ===

  } else if (_stricmp(name, "ontitle") == 0) {
    _onTitle = value;

  } else if (_stricmp(name, "onstation") == 0) {
    _onStation = value;

  } else if (_stricmp(name, "bclk") == 0) {
    _i2s_bclk = _atopin(value);

  } else if (_stricmp(name, "lrc") == 0) {
    _i2s_lrc = _atopin(value);

  } else if (_stricmp(name, "dout") == 0) {
    _i2s_dout = _atopin(value);

  } else {
    ret = false;
  }  // if

  return (ret);
}  // set()


/**
 * @brief Activate the AudioElement.
 */
void AudioElement::start() {
  TRACE("start()");

  if ((_i2s_bclk < 0) || (_i2s_lrc < 0) || (_i2s_dout < 0)) {
    LOGGER_EERR("incomplete configuration");

  } else {
    Element::start();

    __audio = new Audio();
    __audio->setPinout(_i2s_bclk, _i2s_lrc, _i2s_dout);
    __audio->setVolume(_volume);
    __audio->setBalance(_balance);
    __audio->setTone(_low, _mid, _high);

    if (_url.isEmpty()) {
      __audio->stopSong();
    } else {
      __audio->connecttohost(_url.c_str());
    }

    xTaskCreatePinnedToCore(
      audioTask,             /* Function to implement the task */
      "audio",               /* Name of the task */
      16000,                 /* Stack size in words */
      NULL,                  /* Task input parameter */
      2 | portPRIVILEGE_BIT, /* Priority of the task */
      NULL,                  /* Task handle. */
      1                      /* Core where the task should run */
    );
  }
}  // start()


/**
 * @brief push the current value of all properties to the callback.
 */
void AudioElement::pushState(
  std::function<void(const char *pName, const char *eValue)> callback) {
  Element::pushState(callback);
  callback("volume", _printInteger(_volume));
  callback("balance", _printInteger(_balance));
  callback("low", _printInteger(_low));
  callback("mid", _printInteger(_mid));
  callback("high", _printInteger(_high));
}  // pushState()


void AudioElement::term() {
  TRACE("term()");
  if (__audio) {
    __audio->stopSong();
    delete __audio;
  }
  Element::term();
}  // term()


/* ===== Register the Element ===== */

// As long as the Element is project specific or is a element always used
// the registration is placed here without using a register #define.

// When transferred to the HomeDing library a #define like the
// HOMEDING_INCLUDE_XXX should be used to allow the sketch to select the
// available Elements. See <HomeDing.h> the move these lines to AudioElement.h:

// #ifdef HOMEDING_REGISTER
// Register the AudioElement onto the ElementRegistry.
bool AudioElement::registered =
  ElementRegistry::registerElement("audio", AudioElement::create);
// #endif

// End
