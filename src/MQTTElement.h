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
 * * 22.04.2022 running version with publish & subscribe
 */

#pragma once

#include <HomeDing.h>

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
  /**
   * @brief Setup connection parameters and registrations for the given server.
   */
  void _setup();

  /**
   * @brief Connect and re-connect to the server. Subscribe in case of subscriptions required.
   */
  void _connect();

  /**
   * @brief  A message was received by the subscription.
   */
  void _received(String topic, String payload);

  // implementation details
  class MQTTElementImpl *_impl;
};

/* ===== Register the Element ===== */

#ifdef HOMEDING_REGISTER
// Register the MQTTElement onto the ElementRegistry.
bool MQTTElement::registered =
  ElementRegistry::registerElement("mqtt", MQTTElement::create);
#endif
