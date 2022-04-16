/**
 * @file MQTTElement.h
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:
 * * 01.04.2022 created by Matthias Hertel
 */

#pragma once

#include <HomeDing.h>
#include "URI.h"

/**
 * @brief This Element enables communication to MQTT servers to publish a single topic.
 */
class MQTTElement : public Element {
public:
  /**
   * @brief Factory function to create a MQTTElement.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;

  /**
   * @brief Construct a new MQTTElement
   */
  MQTTElement();

  /**
   * @brief initialize a new Element.
   * @param board The board reference.
   */
  virtual void init(Board *board) override;

  /**
   * @brief Set a parameter or property to a new value or start an action.
   * @param name Name of property.
   * @param value Value of property.
   * @return true when property could be changed and the corresponding action
   * could be executed.
   */
  virtual bool set(const char *name, const char *value) override;

  /**
   * @brief Activate the Element.
   * @return true when the Element could be activated.
   * @return false when parameters are not usable.
   */
  virtual void start() override;

  /**
   * @brief Give some processing time to the timer to check for next action.
   */
  virtual void loop() override;

  /**
   * @brief stop all activities and go inactive.
   */
  virtual void term() override;

  /**
   * @brief push the current value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void pushState(
    std::function<void(const char *pName, const char *eValue)> callback) override;

private:
  void _setup();
  void _connect();

  /**
   * @brief The actual value.
   */
  String _value;

  /**
   * @brief The actual value.
   */
  bool _needSending;

  int _bufferSize = 0;  // secure buffer size
  int _errCount = 0;    ///< counting errors to stop communication after too many.

  WiFiClient *_client;  ///< Secure or unsecure client
  class MQTTElementImpl *_impl;

  String _fingerprint;  ///< Server SHA1 fingerprint for secure connections

  // MQTT specific settings
  
  URI _uri;          ///< used mqtt server.
  String _clientID;  ///< the clientID on the mqtt connection
  bool _isSecure;    ///< establish secure connection
  String _topic;     ///< topic path
  int _qos;          ///< Quality Of Service for topic
  bool _retain;      ///< retain value flag

  /**
   * @brief The _xAction holds the actions that is submitted when ...
   */
  // String _xAction;
};
