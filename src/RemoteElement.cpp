/**
 * @file RemoteElement.cpp
 *
 * @brief System Element for the HomeDing Library to send actions to remote
 * devices. This allows starting a remote action on another HomeDing Library
 * based device.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license,
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino.
 *
 * Changelog: see RemoteElement.h
 */

#include <Arduino.h>
#include <Element.h>
#include <Board.h>

#include <RemoteElement.h>

#include <ESP8266WiFi.h>

#define MAX_WAIT_FOR_RESPONSE 12
#define DNS_TIMEOUT (uint32_t)4

/** The TRACE Macro is used for trace output for development/debugging purpose. */
// #define TRACE(...) LOGGER_ETRACE(__VA_ARGS__)
#define TRACE(...)

/**
 * @brief static factory function to create a new RemoteElement.
 * @return RemoteElement* as Element* created element
 */
Element *RemoteElement::create()
{
  return (new RemoteElement());
} // create()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool RemoteElement::set(const char *name, const char *value)
{
  bool ret = true;

  if (_stricmp(name, "host") == 0) {
    _host = value;

  } else if (_stricmp(name, "remoteid") == 0) {
    _remoteId = value;

  } else if (Element::set(name, value)) {
    // ok.

  } else if (active) {
    // may be a remote action
    if (_state != STATE::IDLE) {
      LOGGER_EERR("Remote Element is busy. Action dropped.");
    } else {
      _action = _remoteId + '?' + name + '=' + value;
      _state = STATE::GETIP;
    } // if
  } // if
  return (ret);
} // set()


/**
 * @brief Activate the RemoteElement.
 */
void RemoteElement::start()
{
  if (_host.length() == 0) {
    LOGGER_EERR("no host configured");
  } else if (_remoteId.length() == 0) {
    LOGGER_EERR("no remoteId configured");
  } else {
    Element::start();
  } // if
} // start()


void RemoteElement::_header(String &key, String &value)
{
  TRACE("   =<%s>:<%s>", key.c_str(), value.c_str());
};

/**
 * @brief check the state of the input.
 */
void RemoteElement::loop()
{
  if (_state != STATE::IDLE) {
    TRACE("loop(%d)", _state);
  }

  if ((_state == STATE::GETIP) && (_IPaddr))
    _state = STATE::SENDING; // shortcut, reuse IP address.

  if (_state == STATE::GETIP) {
    // ask for IP address
    TRACE("start DNS...");
    int b = WiFi.hostByName(_host.c_str(), _IPaddr); // , DNS_TIMEOUT);
    TRACE(".got %d %s", b, _IPaddr.toString().c_str());
    if (_IPaddr) {
      _state = STATE::SENDING;
    } else {
      if (!_errNoHostSent) {
        LOGGER_EERR("host %s is unknown.", _host.c_str());
        _errNoHostSent = true;
      }
      _state = STATE::ABORT;
    }

  } else if (_state == STATE::SENDING) {
    // start remote communication,
    _startTime = _board->getSeconds();

    // connect to server
    if (!_httpClient.connect(_IPaddr, 80)) {
      LOGGER_EERR(".no connect");
      _state = STATE::ABORT;
    } else {
      TRACE("connected to server...");
      // 2. send request
      String request("GET /$board/$1 HTTP/1.1\r\nHost: $2\r\nConnection: close\r\n\r\n");
      request.replace("$1", _action);
      request.replace("$2", _host);
      _httpClient.write(request.c_str());
      _state = STATE::CHECK;
    } // if

  } else if (_state == STATE::CHECK) {
    // see if an answer has come back

    if (!_httpClient.connected()) {
      LOGGER_EERR("no connection");
      _state = STATE::ABORT;

    } else if (_httpClient.available() > 0) {
      // Response is available.
      _state = STATE::HEADERS;

    } else {
      // wait or drop.
      if (_board->getSeconds() - _startTime > MAX_WAIT_FOR_RESPONSE) {
        LOGGER_EERR("timed out");
        _state = STATE::ABORT;
      } // if

    } // if

  } else if (_state == STATE::HEADERS) {
    // process response

    // 3. receive header.
    bool headerDone = false;
    int contentLength = 0;

    // Read the header lines of the reply from server
    while (_httpClient.connected() && (!headerDone)) {
      String line = _httpClient.readStringUntil('\n');
      if (!line.endsWith("\r")) {
        // TRACE(" line false.");
        headerDone = true;
        _state = STATE::ABORT;
        break;
      }
      line.replace("\r", "");
      // TRACE(" line=<%s>", line.c_str());

      // isolate header key and value
      int sepPos = line.indexOf(':');
      if (sepPos > 0) {
        String key = line.substring(0, sepPos);
        String val = line.substring(sepPos + 1);
        if (val.startsWith(" ")) {
          val.remove(0, 1);
        }
        // TRACE("   =<%s>:<%s>", key.c_str(), val.c_str());
        _header(key, val);
      }

      if (line.startsWith("Content-Length:")) {
        contentLength = atoi(line.c_str() + 15);
        TRACE(" contentLength=%d", contentLength);

      } else if (line.length() == 0) {
        TRACE(" header end.");
        headerDone = true;
        _state = STATE::BODY;
      } // if
    } // while

    // // 4. receive payload
    // String ret;

    // if (_httpClient.connected() && (contentLength > 0)) {
    //   // LOGGER_EINFO(" received bytes:%d", contentLength);
    //   char *buffer = (char *)malloc(contentLength + 1);
    //   memset(buffer, 0, contentLength + 1);
    //   contentLength -= _httpClient.readBytes(buffer, contentLength);
    //   ret = buffer;
    //   free(buffer);
    // } // if
    _state = STATE::ABORT;

  } // if

  if (_state == STATE::ABORT) {
    _httpClient.stop();
    _state = STATE::IDLE;
  } // if

} // loop()


// Register the RemoteElement in the ElementRegistry.
bool RemoteElement::registered =
    ElementRegistry::registerElement("remote", RemoteElement::create);

// End
