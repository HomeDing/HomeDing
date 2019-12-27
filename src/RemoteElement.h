/**
 * @file RemoteElement.h
 * 
 * @brief System Element for the HomeDing Library to send actions to remote devices.
 * This allows starting a remote action on another HomeDing Library based device.
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
 * * 22.04.2018 created by Matthias Hertel
 * * 17.03.2019 splitting connect into getHostByName and connect by IP.
 */

#ifndef REMOTEELEMENT_H
#define REMOTEELEMENT_H

#include <HomeDing.h>
#include <WiFiClient.h>

/**
 * @brief The RemoteElement is an special Element that creates actions based on
 * a digital IO signal.
 */
class RemoteElement : public Element
{
public:
  /**
   * @brief Factory function to create a RemoteElement.
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
  virtual bool set(const char *name, const char *value);

  /**
   * @brief Activate the RemoteElement.
   * @return true when activation was good.
   * @return false when activation failed.
   */
  virtual void start();

  /**
   * @brief check for completed remote actions.
   */
  virtual void loop();


  // pushState is not required because no dynamic properties

private:
// configuration
  String _host; /** name of the remote device */
  String _remoteId; /** type/id of element in remote device. */

// processing
  int _status; // 0: not active, 1: has sent out, waiting for incomming packet
  IPAddress _IPaddr; // ip of the remote device
  String _action; // next action to be sent

  WiFiClient _httpClient;

  /**
   * @brief time when the request was started.
   */
  unsigned long _startTime;

  bool _errNoHostSent = false; // avoid sending this error all the time
};

#endif // REMOTEELEMENT_H