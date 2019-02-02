/**
 * @file Board.h
 *
 * @brief Interface of the Board class for the HomeDing Library.
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
 * * 23.04.2018 created by Matthias Hertel
 * * 27.04.2018 parameter pushing & loading added.
 * * 31.05.2018 self registering factory implementation.
 * * 25.06.2018 ArduinoJson replace by MicroJsonParser.
 * * 24.08.2018 Environment setting separated from Element configuration.
 * * 26.08.2018 Later display initialization, enabling display configuration.
 * * 09.10.2018 Get time in time_t format.
 * * 11.10.2018 move network initialization into board loop.
 */

// The Board.h file also works as the base import file that contains some
// general definitions.

#ifndef BOARD_H
#define BOARD_H

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <memory>
#include <time.h>

#include <Element.h>
#include <displays/DisplayAdapter.h>

#include "core/Logger.h"

/**
 * @brief The env.json file contains all the settings for registering the device
 * in a local environment. This includes devicename and network settings.
 */
#define ENV_FILENAME "/env.json"

/**
 * @brief The config.json file contains all the settings for the device
 * elements.
 */
#define CONF_FILENAME "/config.json"

#define MAX_LIST_LENGTH 20

/**
 * State of Board
 */
typedef enum {
  // ===== startup operation states
  BOARDSTATE_NONE = 0, // unspecified
  BOARDSTATE_CONFIG =
      1, // read all configurations and create elements. Start SYS Elements
  BOARDSTATE_CONNECT = 2, // try to reconnect to last known network.
  BOARDSTATE_CONFWAIT = 3, // Wait for clicks.
  BOARDSTATE_WAIT = 5, // Wait for network connectivity and clicks.

  // ===== normal operation states
  BOARDSTATE_GREET = 10, // start all NET Elements
  BOARDSTATE_RUN = 12, // run
  // start TIME Elements
  // restart on network lost > 30 secs.

  // ===== config operation states
  BOARDSTATE_STARTCAPTIVE = 21, // Scan local available Networks
  BOARDSTATE_RUNCAPTIVE = 22 // Enable Network Configuration UI

} BoardState;


/**
 * @brief The Board is the platform / functionality that manage all elements
 * required to build an IoT Thing.
 * * create elements defined in the config file.
 * * parameter loading into elements
 * * call loop() for all known elements
 * * dispatch actions.
 *
 * The dispatch method can dispatch multiple actions separated by comma at once.
 * Example: display/active?show=1,digitalout/led?on,device/main?log=*on*
 */
class Board
{
public:
  // ----- Time functionality -----

  /**
   * @brief return the seconds since starting the device.
   */
  static unsigned long getSeconds();

  /**
   * @brief return the seconds since 1.1.1970 00:00:00
   * This method will return 0 when no real time is available.
   */
  static time_t getTime();

  /**
   * @brief Return the seconds of today.
   * This method will return 0 when no real time is available.
   */
  static time_t getTimeOfDay();


  /**
   * @brief Initialize a blank board.
   * @param s The WebServer is always required.
   */
  void init(ESP8266WebServer *s);

  /**
   * @brief Add and config the Elements defined in the config files.
   */
  void addElements();

  /**
   * @brief activate all the Elements by using start().
   */
  void start(Element_StartupMode startupMode);

  /**
   * @brief Give some processing time to all the active elements on the board.
   * One by One.
   */
  void loop();

  /**
   * @brief send all the actions to the right elements.
   * @param action list of actions.
   * @param value the value for $v placeholder.
   */
  void dispatch(const char *action, const char *value = NULL);

  /**
   * @brief send all the actions to the right elements.
   * @param action list of actions.
   * @param value the value for $v placeholder.
   */
  void dispatch(String &action, const char *value = NULL);


  /**
   * @brief Get the state (current values) of a single or all objects
   * @param out Output String for the result.
   * @param path Path of an Element or null to get state of all elements.
   */
  void getState(String &out, const String &path);


  /*
   * @brief Set a single property to a specific value or start an action.
   * @param path Path of an Element.
   * @param property Name of the property
   * @param value New value of the property.
   */
  void setState(String &path, const String &property, const String &value);

  // Display
  DisplayAdapter *display = NULL;

  // System LED
  int sysLED = -1;

  // System Button
  int sysButton = -1;

  // Save Mode
  bool savemode = false;

  // WebServer
  ESP8266WebServer *server;

  /**
   * @brief Get a Element by typename. Returns the first found element.
   * This method can be used to access the singleton Elements like device,
   * display, ota, ...
   * @param elementTypeName type name of element.
   * @return Element* first element in list with this type.
   */
  Element *getElement(const char *elementTypeName);

  static void reboot(bool wipe);

  String deviceName;

  BoardState boardState;

private:
  /**
   * @brief Add another element to the board into the list of created elements.
   * @param id id of element.
   * @param e reference to element.
   */
  void _add(const char *id, Element *e);

  /**
   * @brief Find an Element by the path.
   * @param path
   * @return Element*
   */
  Element *findById(const char *id);

  /**
   * @brief Find an Element by the path.
   * @param path
   * @return Element*
   */
  Element *findById(String &id);

  void _dispatchSingle(String evt);

  // state and timing
  unsigned long configPhaseEnd; // for offering config mode
  unsigned long connectPhaseEnd; // for waiting on net connection
  unsigned long
      _captiveEnd; // terminate/reset captive portal mode after 5 minutes.
  void _newState(BoardState newState);

  bool active = false;
  bool validTime = false;

  int netMode;

  Element *_list2;
  Element *_next2;

  String _actionList;

  static Element *_registered;
};

#endif