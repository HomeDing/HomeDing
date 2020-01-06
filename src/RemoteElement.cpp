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
#include <Board.h>
#include <Element.h>

#include <RemoteElement.h>

#include <ESP8266WiFi.h>

#define MAX_WAIT_FOR_RESPONSE 12
#define DNS_TIMEOUT (uint32_t)4

/** The TRACE Macro is used for trace output for development/debugging purpose. */
#define TRACE(...) LOGGER_ETRACE(__VA_ARGS__)
// #define TRACE(...)

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
  bool ret = HttpClientElement::set(name, value);

  if (!ret) {
    if (_stricmp(name, "remoteId") == 0) {
      _remoteId = value;
      ret = true;
    } else if (active) {
      // may be a remote action
      if (!_action.isEmpty()) {
        LOGGER_EERR("Remote Element is busy. Action dropped.");
      } else {
        _action = _remoteId + '?' + name + '=' + value;
      } // if
      ret = true;
    } // if
  } // if
  return (ret);
} // set()


/**
 * @brief Activate the RemoteElement.
 */
void RemoteElement::start()
{
  if (_remoteId.length() == 0) {
    LOGGER_EERR("no remoteId configured");
  } else {
    Element::start();
  } // if
} // start()


// void RemoteElement::processHeader(String &key, String &value)
// {
//   HttpClientElement::processHeader(key, value);
// };

// void RemoteElement::processBody(char *value)
// {
//   HttpClientElement::processBody(value);
// };

/**
 * @brief check the state of the input.
 */
void RemoteElement::loop()
{
  if ((!HttpClientElement::isActive()) && (!_action.isEmpty())) {
    // start the next action.
    HttpClientElement::set("url", _action.c_str());
    _action.clear();
  } // if

  // process the HttpClientElement current request.
  HttpClientElement::loop();
} // loop()


// Register the RemoteElement in the ElementRegistry.
bool RemoteElement::registered =
    ElementRegistry::registerElement("remote", RemoteElement::create);

// End
