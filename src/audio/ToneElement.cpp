/**
 * @file ToneElement.cpp
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:see ToneElement.h
 */

#if defined(ESP8266)

#include <Arduino.h>
#include <HomeDing.h>

#include "ToneElement.h"

#define TRACE(...)  // LOGGER_ETRACE(__VA_ARGS__)

// http://d1pro/api/state/tone/on?tones=on:b=180:8c+8g

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new ToneElement
 * @return ToneElement* created element
 */
Element *ToneElement::create() {
  return (new ToneElement());
}  // create()


/* ===== RTTTL Functions ===== */

const char *ToneElement::rtttl_noteNames = "c#d#ef#g#a#bh";
const uint16_t ToneElement::rtttl_freq7[13] = { 2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951, 3951 };

uint16_t ToneElement::rtttl_frequency(char note, boolean isSharp, uint16_t scale) {
  char *p = strchr(rtttl_noteNames, note);
  if ((!p) || (note == 'p')) return (0);  // pause
  int idx = p - rtttl_noteNames;

  if (isSharp) idx++;

  if (idx == 12) {
    scale++;
    idx = 0;
  }

  uint16_t f = rtttl_freq7[idx];
  // octaves have half or double frequencies.
  while (scale++ < 7) { f = f / 2; }
  while (scale-- > 7) { f = f * 2; }

  return (f);
}  // rtttl_frequency()


ToneElement::_tone_t ToneElement::rtttl_parse(const char *p) {
  uint16_t d = 0;
  uint16_t o = 0;
  char cNote = 'c';
  boolean isSharp = false;

  while (isDigit(*p)) {
    d = 10 * d + (*p - '0');
    p++;
  }
  if (d == 0) d = _rtttl_d;

  // tone
  if (isalpha(*p)) {
    cNote = *p++;
  }

  // sharp
  if (*p == '#') {
    isSharp = true;
    p++;
  }

  // octave
  while (isDigit(*p)) {
    o = 10 * o + (*p - '0');
    p++;
  }
  if (o == 0) o = _rtttl_o;
  uint16_t f = rtttl_frequency(cNote, isSharp, o);

  uint16_t duration = (60000L * 4 / _rtttl_bpm) / d;
  if (*p == '.') {
    // dotted note
    duration += duration / 2;
  }

  return { f, duration };
} // rtttl_parse()


// convert the rtttl format in a series of tones
void ToneElement::_loadTones(String s) {
  // 3 parts ok.
  ArrayString parts;
  parts.split(s, ':');

  if (parts.size() == 3) {
    String sDefaults = parts[1];
    String sTones = parts[2];

    // set defaults
    _rtttl_bpm = 60;
    _rtttl_d = 4;
    _rtttl_o = 5;

    // parse 2. part: "d=8,o=5,b=112"
    sDefaults.toLowerCase();
    ArrayString defs;
    defs.split(sDefaults.c_str());

    for (int n = 0; n < defs.size(); n++) {
      String aDef = defs[n];
      if (aDef.length() > 2) {
        long n = aDef.substring(2).toInt();
        if (aDef.startsWith("d=")) {
          _rtttl_d = n;
        } else if (aDef.startsWith("o=")) {
          _rtttl_o = n;
        } else if (aDef.startsWith("b=")) {
          _rtttl_bpm = n;
        }
      }
    }

    // parse 3. part: "tone,tone,..."
    sTones.toLowerCase();
    _tonesLength = ListUtils::length(sTones);
    if (_tones) free(_tones);
    _tones = (ToneElement::_tone_t *)malloc(sizeof(ToneElement::_tone_t) * _tonesLength);

    for (int n = 0; n < _tonesLength; n++) {
      String oneTone = ListUtils::at(sTones, n);
      _tones[n] = rtttl_parse(oneTone.c_str());
    }
  }
}  // _loadTones

/* ===== Element functions ===== */

/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool ToneElement::set(const char *name, const char *value) {
  TRACE("set %s=%s", name, value);
  bool ret = true;

  if (Element::set(name, value)) {
    // done.

  } else if (_stricmp(name, "tones") == 0) {
    Serial.printf("Tone: <%s>\n", value);
    String tones = value;
    tones.replace(' ', ',');  // allow ',' and ' '(space) for separation
    _loadTones(tones);

    _nextTone = 0;
    _nextToneTime = 1;  // asap

  } else if (_stricmp(name, "play") == 0) {
    _nextTone = 0;
    _nextToneTime = 1;  // asap

  } else if (_stricmp(name, "pin") == 0) {
    _pin = _atopin(value);

  } else {
    ret = false;
  }  // if

  return (ret);
}  // set()


/**
 * @brief Activate the Element.
 */
void ToneElement::start() {
  TRACE("start()");
  if (_pin >= 0) {
    Element::start();
    _nextToneTime = 0; // don't start on startup
  }  // if
}  // start()


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void ToneElement::loop() {
  // do something
  if (_nextToneTime > 0) {
    unsigned long now = millis();
    if (_nextToneTime == 1) _nextToneTime = now;  // asap

    if (_nextToneTime < now) {

      if (_nextTone < _tonesLength) {
        TRACE("play %d %d", _tones[_nextTone].f, (_tones[_nextTone].d * 9) / 10);
        tone(_pin, _tones[_nextTone].f, (_tones[_nextTone].d * 9) / 10);

        _nextToneTime += _tones[_nextTone++].d;

      } else {
        // done.
        TRACE("done.");
        noTone(_pin);
        _nextToneTime = 0;
      }
    }
  }
}  // loop()

#endif

// End
