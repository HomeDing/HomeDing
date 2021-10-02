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

#include <RFCodes.h>

#define TRACE(...) // LOGGER_ETRACE(__VA_ARGS__)

/* ===== Define local constants and often used strings ===== */


/* ===== Define static class variables here. ====== */

SignalParser sig;
SignalCollector col;

String RFCodesElement::_receivedCode;

bool RFCodesElement::_initialized = false;

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new RFCodesElement
 * @return RFCodesElement* created element
 */
Element *RFCodesElement::create() {
  return (new RFCodesElement());
} // create()


RFCodesElement::RFCodesElement() {
  _clearTime = 10 * 1000; // 10 seconds
  _pinRx = NO_PIN;
  _pinTx = NO_PIN;
}


void RFCodesElement::_receive(const char *newCode) {
  _receivedCode = newCode;
}

/* ===== Element functions ===== */

/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool RFCodesElement::set(const char *name, const char *value) {
  bool ret = true;

  if (_stricmp(name, PROP_VALUE) == 0) {
    _sendValue = value; // will be sent in loop()

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
void RFCodesElement::start() {
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
void RFCodesElement::loop() {
  unsigned long now = millis();

  Element::loop();
  col.loop(); // process received bytes

  if (!_sendValue.isEmpty()) {
    const char *v = _sendValue.c_str();
    TRACE("sending: [%s]", v);
    col.send(v);
    _sendValue.clear();

  } else if (!_receivedCode.isEmpty()) {
    TRACE("received: [%s]", _receivedCode.c_str());

    if (_receivedCode != _lastReceivedCode) {
      TRACE("process: [%s]", _receivedCode.c_str());
      _lastReceivedCode = _receivedCode;
      _receivedMillis = now;
      _receivedTime = time(nullptr);
      _board->dispatch(_valueAction, _receivedCode);
    }
    _receivedCode = "";

  } else if ((!_lastReceivedCode.isEmpty()) && (now > _receivedMillis + _clearTime)) {
    _lastReceivedCode.clear();
    _receivedTime = 0;
  }

} // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void RFCodesElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback) {
  Element::pushState(callback);
  callback("value", _lastReceivedCode.c_str());
  callback("received", String(_receivedTime).c_str());
  // callback("value", _receivedCode.c_str());
} // pushState()


// End
