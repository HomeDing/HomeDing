/**
 * @file HttpClientElement.cpp
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
 * Changelog: see HttpClientElement.h
 */

#include <Arduino.h>
#include <Board.h>
#include <Element.h>

#include <HttpClientElement.h>

#include <ESP8266WiFi.h>

#define TRACE(...) // LOGGER_ETRACE(__VA_ARGS__)

#define MAX_WAIT_FOR_RESPONSE 20
#define DNS_TIMEOUT (uint32_t)4

#define NEWSTATE(n) _state = n;


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool HttpClientElement::set(const char *name, const char *value)
{
  bool ret = true;

  if (_stricmp(name, "host") == 0) {
    _host = value;

  } else if (_stricmp(name, "url") == 0) {
    _url = value;

  } else {
    ret = Element::set(name, value);
  }
  return (ret);
} // set()


/**
 * @brief Activate the HttpClientElement.
 */
void HttpClientElement::start()
{
  if (_host.length() == 0) {
    LOGGER_EERR("no host configured");
  } else {
    Element::start();
  } // if
} // start()


bool HttpClientElement::isActive()
{
  return (_state != STATE::IDLE);
};


void HttpClientElement::processHeader(String &key, String &value)
{
  // TRACE(" =<%s>:<%s>", key.c_str(), value.c_str());
  if (key.equalsIgnoreCase("Content-Length")) {
    _contentLength = atoi(value.c_str());
  }
};

void HttpClientElement::processBody(char *value)
{
};

/**
 * @brief check the state of the input.
 */
void HttpClientElement::loop()
{
  if ((_state != STATE::IDLE) || (!_url.isEmpty())) {
    _board->deferSleepMode();
  } // if

  if (_state == STATE::IDLE) {
    if (!_url.isEmpty()) {
      // TRACE("new URL: %s", _url.c_str());
      NEWSTATE(STATE::GETIP);
    } // if
  } // if

  if (_startTime && (_board->getSeconds() - _startTime > MAX_WAIT_FOR_RESPONSE)) {
    LOGGER_EERR("timeout");
    NEWSTATE(STATE::ABORT);


  } else if ((_state == STATE::GETIP) && (_IPaddr)) {
    NEWSTATE(STATE::SENDING); // shortcut, reuse IP address.

  } else if (_state == STATE::GETIP) {
    // init result
    _contentLength = 0;

    // ask for IP address
    TRACE("start DNS...");
    int b = WiFi.hostByName(_host.c_str(), _IPaddr); // , DNS_TIMEOUT);
    if (_IPaddr) {
      TRACE(".got %d %s", b, _IPaddr.toString().c_str());
      NEWSTATE(STATE::SENDING);
    } else {
      if (!_errNoHostSent) {
        LOGGER_EERR("host %s is unknown.", _host.c_str());
        _errNoHostSent = true;
      }
      NEWSTATE(STATE::ABORT);
    }

  } else if (_state == STATE::SENDING) {
    // start remote communication, connect to server
    if (!_httpClient.connect(_IPaddr, 80)) {
      LOGGER_EERR("nocon");
      NEWSTATE(STATE::ABORT);
    } else {
      // 2. send request
      String request("GET $1 HTTP/1.1\r\nHost: $2\r\nConnection: close\r\n\r\n");
      request.replace("$1", _url);
      request.replace("$2", _host);

      TRACE("request:\n%s", request.c_str());
      _httpClient.write(request.c_str());
      _startTime = _board->getSeconds();
      NEWSTATE(STATE::CHECK);
    } // if

  } else if (_state == STATE::CHECK) {
    // see if an answer has come back
    if (_httpClient.available() > 0) {
      NEWSTATE(STATE::HEADERS);

    } else if (!_httpClient.connected()) {
      LOGGER_EERR("nocon");
      NEWSTATE(STATE::ABORT);
    } // if

  } else if (_state == STATE::HEADERS) {
    // Read the header lines of the reply from server
    while (_httpClient.available()) {
      String line = _httpClient.readStringUntil('\n');

      if (!line.endsWith("\r")) {
        LOGGER_EERR("lineend");
        NEWSTATE(STATE::ABORT);
        break;
      }
      line.replace("\r", "");
      // TRACE("raw: %s", line.c_str());

      if (line.isEmpty()) {
        if (_contentLength) {
          NEWSTATE(STATE::BODY);
        } else {
          NEWSTATE(STATE::ABORT);
        }
        break;

      } else {
        // isolate header key and value
        int sepPos = line.indexOf(':');
        if (sepPos > 0) {
          String key = line.substring(0, sepPos);
          String val = line.substring(sepPos + 1);
          if (val.startsWith(" ")) {
            val.remove(0, 1);
          }
          processHeader(key, val);
        }
      }
      yield();
    } // while available

  } else if (_state == STATE::BODY) {
    // TRACE("body-remaining: %d", _contentLength);
    size_t bufLen = (_contentLength > 1024 ? 1024 : _contentLength);

    if (bufLen) {
      char *buffer = (char *)malloc(bufLen + 1);
      memset(buffer, 0, bufLen + 1);
      size_t r = _httpClient.readBytes(buffer, bufLen);
      // TRACE("  body read %d from %d", r, bufLen);
      _contentLength -= r;
      processBody(buffer);
      free(buffer);
    }
    if (_contentLength == 0) {
      NEWSTATE(STATE::ABORT); // all done.
    }
  } // if

  if (_state == STATE::ABORT) {
    processBody(nullptr); // body passed completely
    _httpClient.stop();
    _url = "";
    _startTime = 0;
    NEWSTATE(STATE::IDLE);
  } // if

} // loop()

// End
