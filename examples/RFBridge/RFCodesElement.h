/**
 * @file RFCodesElement.h
 * @brief The RFCodes Element implements receiving and sending sequences on the 433 MHz using a OOK/ASK transmitter.
* 
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD 3-Clause style license,
 * https://www.mathertel.de/License.aspx.
 * 
 * More information on https://www.mathertel.de/Arduino
 * 
 * Changelog:
 * * 30.07.2018 created by Matthias Hertel
 * * 11.08.2018 using TabRF library
 * * 20.08.2018 TabRF referenced by the Element , no more initialization from external side required.
 * * 20.08.2021 adjusted to updated version of rfcodes library.
 * * 08.04.2021 include receiving RF codes.
 */

#pragma once

/**
 * @brief RFSend Element that produces actions based on timing conditions.
 * @details
@verbatim

The RFCodesElement can ...

@endverbatim
 */

class RFCodesElement : public Element {
public:
  /**
   * @brief Factory function to create a RFCodesElement.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;

  static void _receive(const char *newCode);

  /**
   * @brief Construct a new RFCodesElement
   */
  RFCodesElement();

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

  /**
   * @brief push the current value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void pushState(
      std::function<void(const char *pName, const char *eValue)> callback) override;

private:
  static bool _initialized;

  static String _receivedCode; // code just received, static to be used by the interrupt code

  String _lastReceivedCode; // last processed code for detecting multiple times the same code arrives

  /** The GPIO pin used for the transmitter. */
  int _pinTx;

  /** The GPIO pin used for the receiver. */
  int _pinRx;


  unsigned long _receivedMillis;

  String _sendValue; // code to be sent (inbound value)


  /// time, when the lastCode was received
  time_t _receivedTime;


  /**
   * @brief The time where the same received code is detected as the same code. After this time the code is re-evaluated.
   */
  unsigned long _clearTime; // no process same code in this time

  /**
   * @brief action with received code.
   */
  String _valueAction;
};


// #ifdef HOMEDING_REGISTER
// Register the RFCodesElement onto the ElementRegistry.
bool RFCodesElement::registered =
    ElementRegistry::registerElement("rfcodes", RFCodesElement::create);
// #endif
