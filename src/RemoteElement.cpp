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

#include <Board.h>
#include <ElementRegistry.h>
#include <RemoteElement.h>

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
  LOGGER_ETRACE("set(%s:%s)", name, value);
  bool ret = true;

  if (_stricmp(name, "host") == 0) {
    _host = value;

  } else if (_stricmp(name, "remoteid") == 0) {
    _remoteId = value;

  } else if (!active) {
    ret = Element::set(name, value);
    LOGGER_EERR("inactive");

  } else {
    // must be a remote action
    String url = _remoteId + '?' + name + '=' + value;
    if (_status != 0) {
      LOGGER_EERR("Remote Element is busy. Action dropped.");
    } else {
      _startRemote(url);
    }
  } // if
  return (ret);
} // set()


/**
 * @brief Activate the RemoteElement.
 */
void RemoteElement::start()
{
  bool ret = true;
  if (_host.length() == 0) {
    LOGGER_EERR("missing host configuration");
    ret = false;
  }

  if (_remoteId.length() == 0) {
    LOGGER_EERR("missing remoteId configuration");
    ret = false;
  }

  if (ret) {
    Element::start();
  } // if
} // start()


/**
 * @brief check the state of the input.
 */
void RemoteElement::loop()
{
  // see if a answer has come back
  unsigned long now_ms = millis();

  if ((_status == 1) && (!_httpClient.connected())) {
    LOGGER_EERR("no connection.");
    _httpClient.stop();
    _status = 0;
    return;

  } else if ((_status == 1) && (!_httpClient.available() == 0)) {
    // wait or drop.
    if ((_startTime + 12000) > now_ms) {
      LOGGER_EERR("timed out.");
      _httpClient.stop();
      _status = 0;
      return;
    } // if

  } else if (_status == 1) {

    // 3. receive header.
    bool headerDone = false;
    int contentLength = 0;

    // Read the header lines of the reply from server
    while (_httpClient.connected() && (!headerDone)) {
      String line = _httpClient.readStringUntil('\n');
      LOGGER_ETRACE(" line:%s", line.c_str());
      if (line.startsWith("Content-Length:")) {
        contentLength = _atoi(line.c_str() + 15);
        // contentLength = _atoi(line.substring(15).c_str());
        LOGGER_ETRACE(" contentLength=%d", contentLength);

      } else if (line.equals("\r")) {
        LOGGER_ETRACE(" header end.");
        headerDone = true;
      } // if
    } // while

    // 4. receive payload
    String ret;

    if (_httpClient.connected() && (contentLength > 0)) {
      // LOGGER_EINFO(" received bytes:%d", contentLength);
      char *buffer = (char *)malloc(contentLength + 1);
      memset(buffer, 0, contentLength + 1);
      contentLength -= _httpClient.readBytes(buffer, contentLength);
      ret = buffer;
      free(buffer);
    } // if

    _httpClient.stop();
    _status = 0;

  } // if

} // loop()


void RemoteElement::_startRemote(String url)
{
  LOGGER_ETRACE("_startRemote(%s)", url.c_str());
  _startTime = millis();

  // 1. connect to server
  if (!_httpClient.connect(_host, 80)) {
    LOGGER_EERR("no connection to host %s", _host.c_str());
    _httpClient.stop();
    _status = 0;
    return;
  } // if
  // LOGGER_ETRACE("connected to server");

  // 2. send request
  String request =
      "GET /$board/$id HTTP/1.1\r\nHost $hostConnection: close\r\n\r\n";
  request.replace("$id", url);
  _httpClient.write(request.c_str());

  _status = 1;
  // receive bytes in loop()
} // _startRemote()


// Register the RemoteElement in the ElementRegistry.
bool RemoteElement::registered =
    ElementRegistry::registerElement("remote", RemoteElement::create);

// End
