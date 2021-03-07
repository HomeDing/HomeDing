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
#include <HomeDing.h>

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
        _action = "/$board/" + _remoteId + '?' + name + '=' + value;
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


/**
 * @brief check the state of the input.
 */
void RemoteElement::loop()
{
  if (!_action.isEmpty()) {
    _board->deferSleepMode();

    if (!HttpClientElement::isActive()) {
      // start the next action.
      HttpClientElement::set("url", _action.c_str());
      _action = "";
    } // if
  } // if

  // process the HttpClientElement current request.
  HttpClientElement::loop();
} // loop()

// End
