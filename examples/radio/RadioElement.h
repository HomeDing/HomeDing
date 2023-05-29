/**
 * @file RadioElement.h
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
 * Changelog:
 * * 30.07.2018 created by Matthias Hertel
 */

#pragma once

/**
 * @brief RadioElement implements...
 * @details
@verbatim

The RadioElement can ...

@endverbatim
 */

class RadioElement : public Element {
public:
  /**
   * @brief Factory function to create a RadioElement.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;

  /**
   * @brief Construct a new RadioElement
   */
  RadioElement();

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
   * @return true when the Element could be activated.
   * @return false when parameters are not usable.
   */
  virtual void start() override;

  /**
   * @brief Give some processing time to the timer to check for next action.
   */
  virtual void loop() override;

  /**
   * @brief push the current value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void pushState(
    std::function<void(const char *pName, const char *eValue)> callback) override;

private:
  int _address = 0;  ///< i2c address. 0 means the common i2c addresses are scanned by the chip specific implementation. */
  bool _found = 0;   ///< true when a radio chip was found on i2c bus.

  int _volume = 0;          ///< The current volume.
  bool _mono = true;        ///< mono mode
  bool _mute = false;       ///< muted mode
  bool _softMute = false;   ///< automcatic mute mode
  bool _bassBoost = false;  ///< bass boost mode

  /**
   * @brief The actions that are submitted when a station name is recognized through RDS...
   */
  String _frequencyAction;  ///< actions, when volume has changed
  String _stationAction;    ///< actions, when station name from RDS was received
  String _rdsTextAction;    ///< actions, when new text from rds was received

  int _resetpin = -1;
  int _checkRDS = 500;
  int _checkInfo = 800;
  int _antenna = 0;

  private:
  // implementation details
  class RadioElementImpl *_impl;

};
