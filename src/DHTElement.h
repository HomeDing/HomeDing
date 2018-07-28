/**
 * @file DHTElement.h
 * @brief Input Element of the HomeDong Board Library to read DHT11 and DHT22
 * sensors and create actions.
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
 * * 21.05.2018 created by Matthias Hertel
 */

#ifndef DHTELEMENT_H
#define DHTELEMENT_H

#include "Board.h"
#include "Element.h"
#include <Arduino.h>

#include <DHTesp.h>

/**
 * @brief The DHTElement is an special Element that creates actions based on a
 * digital IO signal.
 */
class DHTElement : public Element
{
public:
  /**
   * @brief Factory function to create a DHTElement.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;

  /**
   * @brief Construct a new DHTElement object.
   */
  DHTElement();

  /**
   * @brief Set a parameter or property to a new value or start an action.
   * @param name Name of property.
   * @param value Value of property.
   * @return true when property could be changed and the corresponding action
   * could be executed.
   */
  virtual bool set(const char *name, const char *value);

  /**
   * @brief Activate the DHTElement.
   * @return true when activation was good.
   * @return false when activation failed.
   */
  virtual void start();

  /**
   * @brief check the state of the DHT values and eventually create actions.
   */
  virtual void loop();

  /**
   * @brief push the current value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void pushState(
      std::function<void(const char *pName, const char *eValue)> callback);

private:
  DHTesp::DHT_MODEL_t _type;
  int _pin;

  /**
   * @brief Last meassured temperature in celsius * 100.
   */
  int _lastTemp; //

  /**
   * @brief Last meassured humidity in percentages.
   */
  int _lastHum;

  /**
   * @brief The _tempAction is emitted when a new temp was read from the DHT
   * sensor.
   */
  String _tempAction;

  /**
   * @brief The _humAction is emitted when a new humidity was read from the DHT
   * sensor.
   */
  String _humAction;

  /**
   * @brief The temperature change and humidity change actions are emitted after
   * some time.
   */
  int _resendTime;

  /**
   * @brief The time between reading 2 probes.
   */
  unsigned long _readTime;


  DHTesp _dht;

  unsigned long _nextRead;
  unsigned long _nextResend;

  /**
   * @brief Format number to string with 2 digits.
   * @param v The value.
   * @param s The char buffer (at least 5+1 chars long)
   * @return char* The char buffer.
   */
  char *_fmt(int v, char *s);

  void _dispatch(String &evt, int value);
};

#endif // DHTELEMENT_H