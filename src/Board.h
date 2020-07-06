/**
 * @file Board.h
 *
 * Interface of the Board class for the HomeDing Library.
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
 * * 10.12.2019 reset counter to enter unsave mode and config mode.
 * * 25.01.2020 device startup actions added.
 */

// The Board.h file also works as the base import file that contains some
// general definitions.

#ifndef BOARD_H
#define BOARD_H

#include <ESP8266WebServer.h>
#include <time.h>

// forward class declarations
class Board;

#include <displays/DisplayAdapter.h>

#include "core/Logger.h"

#define HOMEDING_GREETING "HomeDing"


/**
 * The env.json file contains all the settings for registering the device
 * in a local environment. This includes devicename and network settings.
 */
#define ENV_FILENAME "/env.json"


/**
 * The config.json file contains all the settings for the device elements.
 */
#define CONF_FILENAME "/config.json"


/**
 * State of Board
 */
typedef enum {
  // ===== startup operation states
  BOARDSTATE_NONE = 0, // unspecified
  BOARDSTATE_LOAD = 1, // load configurations and create elements. Start SYS
  BOARDSTATE_CONNECT = 2, // try to reconnect to last known network.
  BOARDSTATE_CONFWAIT = 3, // Wait for clicks.
  BOARDSTATE_WAIT = 5, // Wait for network connectivity and clicks.

  // ===== normal operation states
  BOARDSTATE_GREET = 10, // network established, start NET Elements
  BOARDSTATE_RUN = 12, // in normal operation mode.  
  // start TIME Elements
  // restart on network lost > 30 secs.

  // ===== config operation states
  BOARDSTATE_STARTCAPTIVE = 21, // Scan local available Networks
  BOARDSTATE_RUNCAPTIVE = 22 // Enable Network Configuration UI

} BoardState;


/**
 * The Board is the platform / functionality that manage all elements
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
   * return the seconds since starting the device.
   */
  static unsigned long getSeconds();

  /**
   * return the seconds since 1.1.1970 00:00:00
   * This method will return 0 when no real time is available.
   */
  static time_t getTime() __attribute__((deprecated)); 

  /**
   * Return the seconds of today in localtime.
   * This method will return 0 when no real time is available.
   */
  static time_t getTimeOfDay();


  /**
   * Initialize a blank board.
   * @param s The WebServer is always required.
   */
  void init(ESP8266WebServer *s);

  /**
   * Add and config the Elements defined in the config files.
   */
  void addElements();

  /**
   * activate all the Elements by using start().
   */
  void start(Element_StartupMode startupMode);

  /**
   * Give some processing time to all the active elements on the board.
   * One by One.
   */
  void loop();

  void dispatchItem(String &action, String &values, int item);

  /**
   * send all the actions to the right elements.
   * @param action list of actions.
   * @param value the value for $v placeholder.
   */
  void dispatch(const char *action, const char *value = NULL);

  /**
   * send all the actions to the right elements.
   * @param action list of actions.
   * @param value the value for $v placeholder.
   */
  void dispatch(String &action, const char *value = NULL);

  void dispatch(String &action, int value);
  void dispatch(String &action, String &value);


  /**
   * Get the state (current values) of a single or all objects
   * @param out Output String for the result.
   * @param path Path of an Element or null to get state of all elements.
   */
  void getState(String &out, const String &path);


  /*
   * Set a single property to a specific value or start an action.
   * @param path Path of an Element.
   * @param property Name of the property
   * @param value New value of the property.
   */
  void setState(String &path, const String &property, const String &value);

  /**
   * Display Adapter when a display is configured.
   */
  DisplayAdapter *display = NULL;


  /**
   * System LED pin, will flash during config mode.
   * Typical setting is GPIO2(D4) to use the blue LED on the EPS-12 boards.
   */
  int sysLED = -1;

  
  /**
   * System Button pin, can be used to enter the config mode during startup.
   * Typical use is to use the GPIO0(D3) flash button.
   */
  int sysButton = 0;


  /**
   * Save Mode flag
   */
  bool savemode;

  /**
   * Reset Count
   */
  int _resetCount;

  /**
   * Switch to next network connect mode in msec.
   */
  int nextModeTime = 4 * 1000;

  /**
   * Max. time to wait for a network connection during startup.
   */
  int maxConnectTime = 10 * 1000;


  /**
   * Common I2C settings.
   */
  int I2cSda = PIN_WIRE_SDA;
  int I2cScl = PIN_WIRE_SCL;


  // WebServer
  ESP8266WebServer *server;

  /**
   * Get a Element by typename. Returns the first found element.
   * This method can be used to access the singleton Elements like device,
   * display, ota, ...
   * @param elementTypeName type name of element.
   * @return Element* first element in list with this type.
   */
  Element *getElement(const char *elementTypeName);


  /**
   * Get an Element by type and name. Returns found element.
   * @param elementType type of element.
   * @param elementName name of element.
   * @return Element* element in list with this type/name.
   */
  Element *getElement(const char *elementType, const char *elementName);

  static void reboot(bool wipe);

  /**
   * Display some info to display(optional) and log.
   * @param text1
   * @param text2
   */
  void displayInfo(const char *text1, const char *text2 = NULL);


  /**
   * Network name of the device.
   * Can be configured using the device element `name` property.
   */
  String deviceName;

  /**
   * Start page of the device of no full UTL is given.
   * Can be configured using the device element `homepage` property.
   */
  String homepage = "/index.htm";

  String sysStartAction;
  String startAction;

  BoardState boardState;

private:
  /**
   * Add another element to the board into the list of created elements.
   * @param id id of element.
   * @param e reference to element.
   */
  void _add(const char *id, Element *e);

  /**
   * Find an Element by the path.
   * @param path
   * @return Element*
   */
  Element *findById(const char *id);

  /**
   * Find an Element by the path.
   * @param path
   * @return Element*
   */
  Element *findById(String &id);

  void _dispatchSingle(String evt);

  // state and timing

  unsigned long configPhaseEnd; // millis when current config mode (boardstate) is over, next mode
  unsigned long connectPhaseEnd; // for waiting on net connection
  unsigned long _captiveEnd; // terminate/reset captive portal mode after 5 minutes.
  void _newState(BoardState newState);

  bool active = false;

  /** set to true when any time requesting elements defined. */
  bool hasTimeElements = false;

  /** set to true when all elements have been started. */
  bool startComplete = false;

  /** connection status */
  void _checkNetState();
  wl_status_t _wifi_status;

  /** net connection mode */
  int netMode;

  /** list of active elements */
  Element *_elementList;

  /** next element that will be used in loop() */
  Element *_nextElement;

  String _actionList;

  static Element *_registered;
};

#endif