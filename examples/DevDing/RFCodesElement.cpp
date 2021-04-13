/**
 * @file RFCodesElement.cpp
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
 * Changelog:see RFCodesElement.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include "RFCodesElement.h"

#include <rfcodes.h>

#define TRACE(...) // LOGGER_ETRACE(__VA_ARGS__)

/* ===== Define local constants and often used strings ===== */


/* ===== Define static class variables here. ====== */

SignalParser sig;
SignalCollector col;

int RFCodesElement::_pinTx = NO_PIN;
int RFCodesElement::_pinRx = NO_PIN;
String RFCodesElement::_valueAction;

bool RFCodesElement::_initialized = false;

String RFCodesElement::_receivedCode;
unsigned long RFCodesElement::_receivedMillis = 0; // when code was received
time_t RFCodesElement::_receivedTime;

String RFCodesElement::_lastCode;
unsigned long RFCodesElement::_clearTime = 3 * 1000; // 3 seconds

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new RFCodesElement
 * @return RFCodesElement* created element
 */
Element *RFCodesElement::create()
{
  return (new RFCodesElement());
} // create()


void RFCodesElement::_receive(const char *newCode)
{
  _receivedCode = newCode;
}

/* ===== Element functions ===== */

/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool RFCodesElement::set(const char *name, const char *value)
{
  bool ret = true;

  if (_stricmp(name, PROP_VALUE) == 0) {
    col.send(value);
    LOGGER_INFO("sending: %s", value);
     _lastCode = value;
     _receivedTime = 0;

  } else if (_stricmp(name, ACTION_ONVALUE) == 0) {
    _valueAction = value;

  } else if (_stricmp(name, "pinRx") == 0) {
    _pinRx = _atopin(value);

  } else if (_stricmp(name, "pinTx") == 0) {
    _pinTx = _atopin(value);

  } else if (_stricmp(name, "codeOn") == 0) {
    _codeOn = value;

  } else if (_stricmp(name, "codeOff") == 0) {
    _codeOff = value;

  } else {
    ret = Element::set(name, value);
  } // if

  return (ret);
} // set()


/**
 * @brief Activate the RFCodesElement.
 */
void RFCodesElement::start()
{
  // TRACE("start()");

  // initialize the rfcodes library only once.
  if (!_initialized) {
    TRACE("start rx-pin=%d, tx-pin=%d", _pinRx, _pinTx);

    sig.load(&RFCodes::it1);
    sig.load(&RFCodes::it2);
    // sig.load(&RFCodes::cw);
    sig.dumpTable();

    col.init(&sig, _pinRx, _pinTx); // input at D1 = pin #2 , output at D4, pin # 9
    sig.attachCallback(RFCodesElement::_receive);
    _initialized = true;
  }

  Element::start();
} // start()


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void RFCodesElement::loop()
{
  Element::loop();
  col.loop(); // process received bytes
  
  if (!_receivedCode.isEmpty()) {
    unsigned long now = millis();
    TRACE("receive: [%s]", _receivedCode.c_str());

    if ((_receivedCode != _lastCode) || (now > _receivedMillis + _clearTime)) {
      // TRACE("process: [%s]", _receivedCode.c_str());
      _lastCode = _receivedCode;
      _receivedMillis = now;
      _receivedTime = time(nullptr);
      _board->dispatch(_valueAction, _receivedCode);
    }
    _receivedCode = "";
  }
} // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void RFCodesElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
  callback("value", _lastCode.c_str());
  callback("received", String(_receivedTime).c_str());
  // callback("value", _receivedCode.c_str());
} // pushState()


// End
