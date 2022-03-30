/**
 * @file HttpClientElement.h
 * 
 * @brief System Element for the HomeDing Library to send and receive http requests.
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
 * * 05.01.2020 created by Matthias Hertel
 */

#ifndef HTTPCLIENTELEMENT_H
#define HTTPCLIENTELEMENT_H

#include <HomeDing.h>

/**
 * @brief The RemoteElement is an special Element that creates actions based on
 * a digital IO signal.
 */
class HttpClientElement : public Element
{
public:
  /**
   * @brief Set a parameter or property to a new value or start an action.
   * @param name Name of property.
   * @param value Value of property.
   * @return true when property could be changed and the corresponding action
   * could be executed.
   */
  virtual bool set(const char *name, const char *value) override;

  /**
   * @brief Activate the RemoteElement.
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

  bool isActive();

private:
  /** State of RemoteElement */
  typedef enum {
    IDLE = 0, // no remote activity

    GETIP = 11, // resolve an IP address from hostname.
    SENDING = 12, // send out any request
    CHECK = 13, // check result, if any

    HEADERS = 20, // process header in result.
    BODY = 21, // process body in result.

    ABORT = 99, // stop connection to remote
  } STATE;


  // configuration
  String _host; /** name of the remote device */
  String _url; /** the url for the http request. */

  STATE _state; /// current processing State

  IPAddress _IPaddr; // ip of the remote device

  WiFiClient _httpClient;

  /** content size. */
  int _contentLength;

  /** Time when the request was started. */
  unsigned long _startTime;

  bool _errNoHostSent = false; // avoid sending this error all the time
};

#endif // HTTPCLIENTELEMENT_H