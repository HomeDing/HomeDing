/**
 * @file WeatherFeedElement.h
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
 * Changelog:
 * * 09.01.2029 created by Matthias Hertel
 */

#pragma once

#include <HttpClientElement.h>
#include <WiFiClient.h>

/**
 * @brief The WeatherFeed is an special Element that creates actions based on
 * a digital IO signal.
 */
class WeatherFeedElement : public HttpClientElement
{
public:
  /**
   * @brief Factory function to create a WeatherFeedElement.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;


  /**
   * @brief Set a parameter or property to a new value or start an action.
   * @param name Name of property.
   * @param value Value of property.
   * @return true when property could be changed and the corresponding action
   * could be executed.
   */
  virtual bool set(const char *name, const char *value) override;

  /**
   * @brief Activate the WeatherFeedElement.
   * @return true when activation was good.
   * @return false when activation failed.
   */
  virtual void start() override;

  /**
   * @brief check for completed remote actions.
   */
  virtual void loop() override;


  virtual void processHeader(String &key, String &value);
  virtual void processBody(char *value);

  // pushState is not required because no dynamic properties

private:
  // configuration
  String _remoteId; /** type/id of element in remote device. */
  String _url; /** url from the config. */
  String _location;
  String _apikey;

  unsigned long _readTime = 8 * 60 * 60;
  unsigned long _nextRead;

  String _path;
  String _valueAction; // next action to be sent

  int _count; // correct configures paths and actions.

  ArrayString _paths;
  ArrayString _actions;
};

#ifdef HOMEDING_REGISTER
// Register the RemoteElement in the ElementRegistry.
bool WeatherFeedElement::registered =
    ElementRegistry::registerElement("weatherfeed", WeatherFeedElement::create);

#endif
