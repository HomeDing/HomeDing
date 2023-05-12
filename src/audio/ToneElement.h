/**
 * @file ToneElement.h
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:
 * * 23.04.2023 created by Matthias Hertel
 */

#pragma once

#if defined(ESP8266)

#include <HomeDing.h>

/**
 * @brief
 */
class ToneElement : public Element {
public:
  /**
   * @brief Factory function to create a ToneElement.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;

  /**
   * @brief Set a parameter or property to a new value or start an action.
   * @param name Name of property.
   * @param value Value of property.
   * @return true when property could be changed and the corresponding action
   * could be executed.
   */
  virtual bool set(const char *name, const char *value) override;

  /**
   * @brief Activate the Element.
   */
  virtual void start() override;

  /**
   * @brief Give some processing time to the timer to check for next action.
   */
  virtual void loop() override;

private:
  // One tone to play
  struct _tone_t {
    uint16_t f;
    uint16_t d;
  };

  // The names of the notes in the 7th octave.
  static const char *rtttl_noteNames;

  // The frequencies of the notes in the 7th octave. (b and h are equal)
  static const uint16_t rtttl_freq7[13];

  // pin for tone output.
  int _pin = -1;

  // rtttl definition of the tone sequence.
  String _sTones = "";

  uint16_t _rtttl_d;    // default tone duration
  uint16_t _rtttl_o;    // default tone octave
  uint16_t _rtttl_bpm;  // bpm

  // current loaded tone sequence
  ToneElement::_tone_t *_tones = nullptr;
  uint16_t _tonesLength = 0;

  // current item in tones
  uint16_t _nextTone = -1;

  // time for the next tone.
  unsigned long _nextToneTime = 0;

  // convert the rtttl format in a series of tones
  void _loadTones(String s);

  // parse one tone in the list
  _tone_t rtttl_parse(const char *p);

  // calculate the frequency of one tone.
  uint16_t rtttl_frequency(char note, boolean isSharp, uint16_t scale);
};

/* ===== Register the Element ===== */

 #ifdef HOMEDING_REGISTER
// Register the ToneElement onto the ElementRegistry.
bool ToneElement::registered =
  ElementRegistry::registerElement("tone", ToneElement::create);
#endif

#endif
