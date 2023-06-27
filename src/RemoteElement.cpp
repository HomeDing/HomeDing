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

#include <WiFiClient.h>
#include <ArrayString.h>

#if !defined(TRACE)
#define TRACE(...)  // LOGGER_ETRACE(__VA_ARGS__)
#endif

/**
 * @brief static factory function to create a new RemoteElement.
 * @return RemoteElement* as Element* created element
 */
Element *RemoteElement::create() {
  return (new RemoteElement());
}  // create()


void RemoteElement::dispatchAction(String &targetId, String &name, String &value) {
  TRACE("dispatch '%s?%s=%s", targetId.c_str(), name.c_str(), value.c_str());

  String url = "/api/state/$1?$2=$3";
  url.replace("$1", targetId);
  url.replace("$2", name);
  url.replace("$3", value);

  if (isActive()) {
    LOGGER_ETRACE("queued <%s>", url.c_str());
    _queue.push(url);

  } else {
    TRACE("get %s", url.c_str());
    set("url", url.c_str());
  }  // if
}


void RemoteElement::loop() {
  HttpClientElement::loop();
  if ((!_queue.empty()) && (!isActive())) {
    String url = _queue.pop();

    TRACE("get %s", url.c_str());
    set("url", url.c_str());
  }
}

// End
