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

#ifndef RFCodesElement_H_
#define RFCodesElement_H_

#include <HomeDing.h>

/**
 * @brief RFSend Element that produces actions based on timing conditions.
 * @details
@verbatim

The RFCodesElement can ...

@endverbatim
 */

class RFCodesElement : public Element
{
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

  /** 
   * The GPIO pin used for the transmitter.
   */
  static int _pinTx;

  /** 
   * The GPIO pin used for the receiver.
   */
  static int _pinRx;

  static void _receive(const char *newCode);

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

  static String _receivedCode; // code just received
  
  /// time, when the lastCode was received
  static unsigned long _receivedMillis;
  static time_t _receivedTime;

  static String _lastCode; // last processed code
  static unsigned long _clearTime; // no process codes in this time

 static String _valueAction;

  /**
   * @brief The actual value.
   */
  String _codeOn;
  String _codeOff;
  bool _lastValue;
};


// #ifdef HOMEDING_REGISTER
// Register the RFCodesElement onto the ElementRegistry.
bool RFCodesElement::registered =
    ElementRegistry::registerElement("rfcodes", RFCodesElement::create);
// #endif


#endif // RFCodesElement_H_