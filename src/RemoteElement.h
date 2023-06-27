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
 * * 05.01.2020 using HttpClientElement
 * * 27.06.2023 queueing events to the same remote
 */

#include <HttpClientElement.h>

/**
 * @brief The RemoteElement is an special Element that creates actions based on
 * a digital IO signal.
 */
class RemoteElement : public HttpClientElement {
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


  /// @brief Give some processing time to the timer to check for next action.
  virtual void loop() override;


  /// @brief Dispatch an actio to the remote device.
  /// @param targetId element type and id in the remote device
  /// @param name name of the property/action
  /// @param value value of the property/action
  void dispatchAction(String &targetId, String &name, String &value);

private:
  /// @brief queued actions
  ArrayString _queue;
};

#ifdef HOMEDING_REGISTER
// Register the RemoteElement in the ElementRegistry.
bool RemoteElement::registered =
  ElementRegistry::registerElement("remote", RemoteElement::create);
#endif
