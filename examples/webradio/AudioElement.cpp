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

#define AUDIOCMD_NONE 0x0000
#define AUDIOCMD_VOLUME 0x0001
#define AUDIOCMD_BALANCE 0x0002
#define AUDIOCMD_TONE 0x0004
#define AUDIOCMD_MONO 0x0008
#define AUDIOCMD_URL 0x0100

// These references are set in static variables to make it available in the background audio task.

AudioElement *__element = nullptr;
Audio *__audio = nullptr;
uint32_t __cmd = AUDIOCMD_NONE;


// ===== Background task for audio processing ======

// use some weak functions from audio library

// callbacks
void __audio_info(Audio::msg_t m) {
  Serial.printf("audio::%s(%d): %s\n", m.s, m.e, m.msg);

  if (m.e == Audio::event_t::evt_info) {
    // trace info
    Logger::LoggerEPrint(__element, LOGGER_LEVEL_INFO, "audio::info %s=%s", m.s, m.msg);

  } else if (m.e == Audio::event_t::evt_streamtitle) {
    __element->_title = m.msg;
    HomeDing::Actions::push(__element->_onTitle, m.msg);

  } else if (m.e == Audio::event_t::evt_name) {
    __element->_station = m.msg;
    HomeDing::Actions::push(__element->_onStation, m.msg);

  } else if (m.e == Audio::event_t::evt_bitrate) {
    __element->_bitrate = m.s;

    // } else if (m.e == Audio::event_t::evt_icydescription{
    //   __element->_description = m.msg;

    // } else if (m.e == Audio::event_t::evt_id3data) {

    // } else if (m.e == Audio::event_t::evt_image) {
    // // log_n("image: %s\n", file.name());

    // } else if (m.e == Audio::event_t::evt_eof) {
    // } else if (m.e == Audio::event_t::evt_log) {

    // evt_icyurl, evt_icylogo, evt_lasthost, evt_lyrics
  }
}  // __audio_info()


void audioTask(void *parameter) {

  __audio->setVolume(__element->_volume);
  __audio->setBalance(__element->_balance);
  __audio->setTone(__element->_low, __element->_mid, __element->_high);
  __audio->forceMono(__element->_mono);

  while (true) {
    if (__cmd != AUDIOCMD_NONE) {
      uint32_t cmd = __cmd;
      __cmd = AUDIOCMD_NONE;

      if (cmd & AUDIOCMD_URL) {
        // start a new URL.
        __audio->stopSong();

        __element->_title.clear();
        HomeDing::Actions::push(__element->_onTitle, "");
        __element->_station.clear();
        HomeDing::Actions::push(__element->_onStation, "");

        if (!__element->_url.isEmpty()) {
          log_n(".1 %s\n", __element->_url.c_str());
          __audio->connecttohost(__element->_url.c_str());
        }
      }

      if (cmd & AUDIOCMD_VOLUME) __audio->setVolume(__element->_volume);
      if (cmd & AUDIOCMD_BALANCE) __audio->setBalance(__element->_balance);
      if (cmd & AUDIOCMD_TONE) __audio->setTone(__element->_low, __element->_mid, __element->_high);
      if (cmd & AUDIOCMD_MONO) __audio->forceMono(__element->_mono);
    }  // if
    __audio->loop();
    if (!__audio->isRunning()) {
      sleep(1);
    }
    vTaskDelay(0);
  }  // while
}



/* ===== Element functions ===== */

void AudioElement::init(Board *board) {
  TRACE("init()");
  Element::init(board);
  // do something here like initialization

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
    __cmd |= AUDIOCMD_URL;

  } else if (_stricmp(name, "volume") == 0) {
    int v = _atoi(value);
    _volume = constrain(v, 0, 21);
    __cmd |= AUDIOCMD_VOLUME;

  } else if (_stricmp(name, "balance") == 0) {
    int v = _atoi(value);
    _balance = constrain(v, -16, 16);
    __cmd |= AUDIOCMD_BALANCE;

  } else if (_stricmp(name, "low") == 0) {
    int v = _atoi(value);
    _low = constrain(v, -20, 6);
    __cmd |= AUDIOCMD_TONE;

  } else if (_stricmp(name, "mid") == 0) {
    int v = _atoi(value);
    _mid = constrain(v, -20, 6);
    __cmd |= AUDIOCMD_TONE;

  } else if (_stricmp(name, "high") == 0) {
    int v = _atoi(value);
    _high = constrain(v, -20, 6);
    __cmd |= AUDIOCMD_TONE;

  } else if (_stricmp(name, "mono") == 0) {
    _mono = _atob(value);
    __cmd |= AUDIOCMD_MONO;

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

    __audio = new Audio(I2S_NUM_0);
    Audio::audio_info_callback = __audio_info;

    __audio->setPinout(_i2s_bclk, _i2s_lrc, _i2s_dout);
    __audio->setVolume(0);

    __cmd |= AUDIOCMD_VOLUME | AUDIOCMD_BALANCE | AUDIOCMD_TONE | AUDIOCMD_MONO | AUDIOCMD_URL;

    xTaskCreatePinnedToCore(
      audioTask, /* Function to implement the task */
      "audio",   /* Name of the task */
      16000,     /* Stack size in words */
      NULL,      /* Task input parameter */
      // 2 | portPRIVILEGE_BIT, /* Priority of the task */
      1 | portPRIVILEGE_BIT, /* Priority of the task */
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
  callback("url", _url.c_str());
  callback("station", _station.c_str());
  callback("streamtitle", _title.c_str());
  callback("volume", _printInteger(_volume));
  callback("balance", _printInteger(_balance));
  callback("low", _printInteger(_low));
  callback("mid", _printInteger(_mid));
  callback("high", _printInteger(_high));
  callback("mono", _printBoolean(_mono));
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
