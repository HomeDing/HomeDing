/**
 * @file DigitalSignalElement.h
 *
 * @brief Core Input Element for the HomeDing Library typically used for short pulse signals.
 * This element uses an interrupt driven approach to capture signal changes.
 * An output signal is generated with a given minimal duration as ong as the input changes.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD 3-Clause style license,
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino.
 *
 * Changelog:
 * * 30.10.2020 created by Matthias Hertel
 */

#ifndef DIGITALIN_H
#define DIGITALIN_H

#include <HomeDing.h>

#define BUTTON_TYPE_LEVEL 0x00
#define BUTTON_TYPE_TOGGLE 0x01

// class OneButton;

/**
 * @brief The DigitalSignalElement is an special Element that creates actions based
 * on a digital IO signal.
 */
class DigitalSignalElement : public Element
{
public:
  /**
   * @brief Factory function to create a DigitalSignalElement.
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
   * @return true when activation was good.
   * @return false when activation failed.
   */
  virtual void start() override;

  /**
   * @brief check the state of the input signal and eventually emit actions.
   */
  virtual void loop() override;

  /**
   * @brief push the current value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void pushState(
      std::function<void(const char *pName, const char *eValue)> callback) override;

private:
  // ----- static interrupt stuff -----
  // we have up to 8 interrupt routines at hand to watch up to 8 input GPIOs for signals,

  static uint _usedSignals;
  static unsigned long _signalCount[8];

  static ICACHE_RAM_ATTR void onSignal0();
  static ICACHE_RAM_ATTR void onSignal1();
  static ICACHE_RAM_ATTR void onSignal2();
  static ICACHE_RAM_ATTR void onSignal3();
  static ICACHE_RAM_ATTR void onSignal4();
  static ICACHE_RAM_ATTR void onSignal5();
  static ICACHE_RAM_ATTR void onSignal6();
  static ICACHE_RAM_ATTR void onSignal7();

  // ----- private element members -----

  /**
   * @brief GPIO pin to be used for physical signal.
   */
  int _pin = -1;

  /**
   * @brief Control level logic for input at _pin. When true: physical HIGH level will produce a logical LOW value.
   */
  bool _inverse = false;

  /**
   * @brief Control internal pullup. When true the internal pullup is activated.
   */
  bool _pullup = false;

  /**
   * @brief the signal counter offset used.
   */
  int _signalOff = -1;

  // last analyzed signal time.
  unsigned long _lastSignalTime;

  // last analyzed signal counter value.
  unsigned long _lastSignalCount;


  // members for pulse output support
  // last analyzed time when frequency was reported.
  bool _pulseValue;
  unsigned long _pulseStart;
  unsigned long _pulseDuration = 1000;

  /**
   * @brief The _highAction is emitted when the logical input level is going
   * from LOW to HIGH.
   */
  String _highAction;

  /**
   * @brief The _lowAction is emitted when the logical input level is going from
   * HIGH to LOW.
   */
  String _lowAction;

  /**
   * @brief The _valueAction is emitted every time the logical input level
   * changed.
   */
  String _valueAction;
};

#ifdef HOMEDING_REGISTER
// Register the DigitalSignalElement onto the ElementRegistry.
// bool DigitalSignalElement::registered =
//     ElementRegistry::registerElement("digitalsignal", DigitalSignalElement::create);
#endif

#endif // DIGITALIN_H