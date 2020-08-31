/**
 * @file WeatherFeed.cpp
 *
 * @brief Element for looking up weather conditions or forecasts from the openweathermap.org web site.
 * The service is polled from time to time and the actual value from the returned data is send as an action
 * similar to a local attached sensor.
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
 * Changelog: see WeatherFeedElement.h
 */

#include <Arduino.h>
#include <Board.h>
#include <Element.h>

#include <WeatherFeedElement.h>

#include "MicroJsonParser.h"

#include <ESP8266WiFi.h>

/** The TRACE Macro is used for trace output for development/debugging purpose. */
// #define TRACE(...) LOGGER_ETRACE(__VA_ARGS__)
#define TRACE(...)

MicroJson *mj = nullptr;

/**
 * @brief static factory function to create a new WeatherFeed.
 * @return WeatherFeed* as Element* created element
 */
Element *WeatherFeedElement::create()
{
  return (new WeatherFeedElement());
} // create()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool WeatherFeedElement::set(const char *name, const char *value)
{
  bool ret = HttpClientElement::set(name, value); // host, url, loglevel, ...

  if (!ret) {
    if (_stricmp(name, "get") == 0) {
      _url = value; // url with parameters
    } else if (_stricmp(name, "key") == 0) {
      _apikey = value;
    } else if (_stricmp(name, "loc") == 0) {
      _location = value;
    } else if (_stricmp(name, "readtime") == 0) {
      _readTime = _atotime(value);

      // save all paths and actions in the vectors.
    } else if (_stricmp(name, "path") == 0) {
      _paths.push_back(value);
    } else if (_stricmp(name, ACTION_ONVALUE) == 0) {
      _actions.push_back(value);

    } // if
  } // if
  return (true);
} // set()


/**
 * @brief Activate the WeatherFeed.
 */
void WeatherFeedElement::start()
{
  unsigned int now = _board->getSeconds();
  _nextRead = now + 2; // now + min. 2 sec., don't hurry
  HttpClientElement::start();

  // correct configures paths and actions.
  _count = std::min(_paths.size(), _actions.size());

  // for (int n = 0; n < _count; n++) {
  //   TRACE("path %s => action %s", _paths[n].c_str(), _actions[n].c_str());
  // }
} // start()


void WeatherFeedElement::processHeader(String &key, String &value)
{
  TRACE("Header: <%s>:<%s>", key.c_str(), value.c_str());
  HttpClientElement::processHeader(key, value);
};

void WeatherFeedElement::processBody(char *value)
{
  HttpClientElement::processBody(value);
  if (!value) {
    delete mj;
    mj = nullptr;
    TRACE("parser deleted");

  } else {
    TRACE("body(%d)", strlen(value));
    if (!mj) {
      mj = new (std::nothrow) MicroJson(
          [this](int level, char *path, char *value) {
            if (path && value) {
              // LOGGER_INFO("<%s>=%s", path, value);
              // test all defined paths
              for (int n = 0; n < _count; n++) {
                if (_paths[n].equalsIgnoreCase(path)) {
                  _board->dispatch(_actions[n], value);
                }
              } // for
            } // if
          });
    }
    if (mj) {
      mj->parse(value);
    }
  }
};


void WeatherFeedElement::loop()
{
  unsigned int now = _board->getSeconds();

  if ((_nextRead <= now) && (!HttpClientElement::isActive())) {
    // start the next polling, build the real url
    String url(_url);
    url.replace("${loc}", _location);
    url.replace("${key}", _apikey);

    // url = "/home/state.json";
    TRACE("Fetch: <%s>", url.c_str());

    HttpClientElement::set("url", url.c_str());
    _nextRead = now + _readTime;
  } // if

  // process the HttpClientElement current request.
  HttpClientElement::loop();
} // loop()


// End
