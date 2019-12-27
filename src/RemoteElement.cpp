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
#define TRACE(...) LOGGER_ETRACE(__VA_ARGS__)
// #define TRACE(...)

/**
 * State of RemoteElement
 */
typedef enum {
  // ===== startup operation states
  REMOTESTATE_IDLE = 0, // no remote activity

  REMOTESTATE_START = 11, // action was received, now start
  REMOTESTATE_OUT = 12,
  REMOTESTATE_SENT = 13, // try to reconnect to last known network.

  REMOTESTATE_ABORT = 99, // stop connection to remote

} REMOTESTATE;


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

  } else if (_stricmp(name, "loglevel") == 0) {
    ret = Element::set(name, value);

  } else if (!active) {
    ret = Element::set(name, value);
    LOGGER_EERR("inactive");

  } else {
    // must be a remote action
    if (_status != REMOTESTATE_IDLE) {
      LOGGER_EERR("Remote Element is busy. Action dropped.");
    } else {
      _action = _remoteId + '?' + name + '=' + value;
      _status = REMOTESTATE_START;
      // _startRemote(_action);
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


/**
 * @brief check the state of the input.
 */
void RemoteElement::loop()
{
  if ((_status == REMOTESTATE_START) && (_IPaddr))
    _status = REMOTESTATE_OUT; // shortcut.

  if (_status == REMOTESTATE_START) {
    // ask for IP address
    TRACE("start DNS...");
    int b = WiFi.hostByName(_host.c_str(), _IPaddr); // , DNS_TIMEOUT);
    TRACE(".got %d %s", b, _IPaddr.toString().c_str());
    if (_IPaddr) {
      _status = REMOTESTATE_OUT;
    } else {
      if (!_errNoHostSent) {
        LOGGER_EERR("host %s is unknown.", _host.c_str());
        _errNoHostSent = true;
      }
      _status = REMOTESTATE_ABORT;
    }

  } else if (_status == REMOTESTATE_OUT) {
    // start remote communication,
    _startTime = _board->getSeconds();

    // connect to server
    if (!_httpClient.connect(_IPaddr, 80)) {
      LOGGER_EERR(".no connect");
      _status = REMOTESTATE_ABORT;
    } else {
      TRACE("connected to server...");
      // 2. send request
      String request =
          "GET /$board/$1 HTTP/1.1\r\nHost: $2\r\nConnection: close\r\n\r\n";
      request.replace("$1", _action);
      request.replace("$2", _host);
      _httpClient.write(request.c_str());
      _status = REMOTESTATE_SENT;
    } // if

  } else if (_status == REMOTESTATE_SENT) {
    // see if a answer has come back

    if (!_httpClient.connected()) {
      LOGGER_EERR("no connection");
      _status = REMOTESTATE_ABORT;

    } else if (_httpClient.available() == 0) {
      // wait or drop.
      if (_board->getSeconds() - _startTime > MAX_WAIT_FOR_RESPONSE) {
        LOGGER_EERR("timed out");
        _status = REMOTESTATE_ABORT;
      } // if

    } else {
      // Response is available.

      // 3. receive header.
      bool headerDone = false;
      int contentLength = 0;

      // Read the header lines of the reply from server
      while (_httpClient.connected() && (!headerDone)) {
        String line = _httpClient.readStringUntil('\n');
        // TRACE(" line:%s", line.c_str());
        if (line.startsWith("Content-Length:")) {
          contentLength = atoi(line.c_str() + 15);
          // TRACE(" contentLength=%d", contentLength);

        } else if (line.equals("\r")) {
          // TRACE(" header end.");
          headerDone = true;
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
      _status = REMOTESTATE_ABORT;

    } // if

  } // if

  if (_status == REMOTESTATE_ABORT) {
    _httpClient.stop();
    _status = REMOTESTATE_IDLE;
  } // if

} // loop()


// Register the RemoteElement in the ElementRegistry.
bool RemoteElement::registered =
    ElementRegistry::registerElement("remote", RemoteElement::create);

// End
