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
 * Changelog: see WeatherFeed.h
 */

#include <Arduino.h>
#include <Board.h>
#include <Element.h>

#include <WeatherFeed.h>

#include <ESP8266WiFi.h>

/** The TRACE Macro is used for trace output for development/debugging purpose. */
#define TRACE(...) LOGGER_ETRACE(__VA_ARGS__)
// #define TRACE(...)

/**
 * @brief static factory function to create a new WeatherFeed.
 * @return WeatherFeed* as Element* created element
 */
Element *WeatherFeedElement::create()
{
  return (new WeatherFeedElement());
} // create()


void WeatherFeedElement::init(Board *board)
{
  HttpClientElement::init(board);
  // initialization of default values
  // set("host", "api.openweathermap.org");
  _readTime = 24 * 60 * 60; // poll once a day
} // init()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool WeatherFeedElement::set(const char *name, const char *value)
{
  bool ret = true;

  if (_stricmp(name, "key") == 0) {
    _apikey = value;
  } else if (_stricmp(name, "loc") == 0) {
    _location = value;
  } else if (_stricmp(name, "url") == 0) {
    _url = value;
  } else if (_stricmp(name, ACTION_ONVALUE) == 0) {
    _valueAction = value;
  } else if (_stricmp(name, "readtime") == 0) {
    _readTime = _atotime(value);
  } else {
    ret = HttpClientElement::set(name, value); // host, ...
  } // if
  return (ret);
} // set()


/**
 * @brief Activate the WeatherFeed.
 */
void WeatherFeedElement::start()
{
  unsigned int now = _board->getSeconds();
  _nextRead = now + 2; // now + min. 2 sec., don't hurry
  HttpClientElement::start();
} // start()


void WeatherFeedElement::processHeader(String &key, String &value)
{
  // TRACE("Header: <%s>:<%s>", key.c_str(), value.c_str());
  HttpClientElement::processHeader(key, value);
};

void WeatherFeedElement::processBody(char *value)
{
  TRACE("body: <%s>", value);
  // HttpClientElement::processBody(value);
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
