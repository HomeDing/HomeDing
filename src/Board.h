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
 * * 10.12.2019 reset counter to enter unsafe mode and config mode.
 * * 25.01.2020 device startup actions added.
 * * 31.08.2020 enable TRACE ouput using Macros to reduce production code size.
 * * 03.09.2020 forEach iterator over all elements added.
 * * 09.03.2021 less sharing class members in favor to methods.
 * * 02.09.2021 advertise web server in mDNS.
 */

// The Board.h file also works as the base import file that contains some
// general definitions and SDK/processor specific includes.

#ifndef BOARD_H
#define BOARD_H

#if defined(ESP32)
#include <WebServer.h>
#include <WiFi.h>
#include <LittleFS.h>
#define FILESYSTEM fs::LittleFSFS

#elif defined(ESP8266)
#include <FS.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#define WebServer ESP8266WebServer
#define FILESYSTEM FS

#endif

#include <WiFiClient.h>

#include <time.h>

#define UNUSED __attribute__((unused))

// forward class declarations
class Board;

#include <Element.h>
#include <displays/DisplayElement.h>
#include <displays/DisplayAdapter.h>

#include <StateElement.h>
#include <core/Logger.h>
#include <core/RTCVariables.h>

#define HOMEDING_GREETING "HomeDing"

// ===== TIMING DEBUG OUTPUT HELPERs =====
// can be controlled using the "Debug port" configuration setting. Set to none to remove TRACE calls.

#ifdef DEBUG_ESP_PORT

/** The TRACE Macros for creating output with timing hints: */
#define TRACE_START                      // unsigned long __TRACE_START_TIME = millis();
#define TRACE_END                        // unsigned long __TRACE_END_TIME = millis();
#define TRACE_TIME                       // (__TRACE_END_TIME - __TRACE_START_TIME)
#define TRACE_TIMEPRINT(topic, id, min)  // if (TRACE_TIME >= min) LOGGER_JUSTINFO(topic " %s (%dms)", id, TRACE_TIME);

#else
// #define TRACE(...)
#define TRACE_START
#define TRACE_END
#define TRACE_TIME
#define TRACE_TIMEPRINT(topic, id, min)
#endif

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
 * The $net.txt file contains the configured network and credentials.
 */
#define NET_FILENAME "/$net.txt"


/**
 * @brief iterator callback function.
 */
typedef std::function<void(Element *e)>
  ElementCallbackFn;


/** inline yield function for cooperative multitasking platforms. */
inline __attribute__((always_inline)) void hd_yield() {
  // delay(1);
  optimistic_yield(100);
}


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
class Board {

  /** States of the board */
  enum BOARDSTATE : int {
    // ===== startup operation states
    NONE = 0,  // unspecified
    LOAD = 1,  // load configurations and create elements. Start SYS Elements

    CONNECT = 2,  // define how to connect, AUTO, PSK or PASSWD
    WAITNET = 3,  // Wait for network connectivity or configuration request.
    WAIT = 5,     // network is connected but wait for configuration request.

    // ===== normal operation states
    GREET = 10,  // network established, start NET Elements
    RUN = 12,    // in normal operation mode.

    SLEEP = 18,  // start sleep mode.

    // ===== config operation states
    STARTCAPTIVE = 21,  // Scan local available Networks
    RUNCAPTIVE = 22     // Enable Network Configuration UI
  };


public:
  // Major and minor version e.g. 1.00 in sync with version in Arduino library definition.
  const char *version = "0.90";

  // The build name defined by the main sketch e.g. "minimal", "standard", "radio"
  String build;

  // ----- Time functionality -----

  /** The result of the millis() function at the start of loop().
   * This can be used for all time / durtion calculations except high precission ones.
   */
  unsigned long nowMillis;

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
  void init(WebServer *s, FILESYSTEM *fs, const char *buildName = "");

  /**
   * Directly add an element to the list of created elements.
   * @param id id of element.
   * @param e reference to element.
   */
  void add(const char *id, Element *e);


  // ===== Board state functionality =====

  /** Return true when board is runing in captive mode. */
  bool isCaptiveMode();

  /** Continue the Captive Mode when activity is detected. */
  void keepCaptiveMode();


  // ===== Start Elements functionality =====

  /**
   * activate all the Elements by using start().
   */
  void start(Element_StartupMode startupMode);

  /**
   * Give some processing time to all the active elements on the board.
   * One by One.
   */
  void loop();


  // ===== low power / sleep mode =====

  // start deep sleep mode in some milliseconds
  void startSleep();

  // do not start deep sleep mode until reboot
  void cancelSleep();

  /**
   * set the duration of a deep sleep.
   * @param secs duration in seconds.
   */
  void setSleepTime(unsigned long secs);


  /**
   * do not start sleep mode because element is active.
   */
  void deferSleepMode();


  // ===== queue / process / dispatch actions =====

  /**
   * check if the dispatch queue is empty
   * @return true when no action is in queue.
   */
  bool queueIsEmpty();

  /**
   * send all the actions to the right elements.
   * @param action list of actions.
   * @param value the value for $v placeholder.
   * @param item use the n-th item of the value.
   */
  void dispatchItem(String &action, String &values, int n);

  /**
   * send all the actions to the right elements.
   * @param action list of actions.
   * @param value the value for $v placeholder.
   */
  void dispatch(String &action, const char *value = nullptr);

  /**
   * send all the actions to the right elements.
   * @param action list of actions.
   * @param value the value for $v placeholder.
   */
  void dispatch(String &action, int value);

  /**
   * send all the actions to the right elements.
   * @param action list of actions.
   * @param value the value for $v placeholder.
   */
  void dispatch(const String &action, const String &value);

  /**
   * @brief Dispatch an action without queueing it.
   * @param action The action string.
   */
  void dispatchAction(String action);


  // ===== state of elements =====

  /**
   * Get the state (current values) of a single or all objects
   * @param out Output String for the result.
   * @param path Path of an Element or null to get state of all elements.
   */
  void getState(String &out, const String &path);


  /**
   * Display Adapter when a display is configured.
   */
  DisplayAdapter *display = NULL;


  /**
   * State Element to save state data.
   */
  StateElement *state = NULL;


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
   * Safe Mode flag
   */
  bool isSafeMode;

  /**
   * Switch to next network connect mode in msec.
   */
  int maxNetConnextTime = 12 * 1000;

  /**
   * Min. time to wait for a configuration mode request.
   */
  int minConfigTime = 6 * 1000;


#if defined(ESP32)
  /**
   * next free led channel to be used on ESP32
   */
  int nextLedChannel = 0;
#endif

  /**
   * Common I2C settings.
   */
  int I2cSda = -1;
  int I2cScl = -1;

  /** Service discovery enabled */
  bool mDNS_sd = true;

  /** WebServer instance */
  WebServer *server;

  /** FileSystem instance */
  FILESYSTEM *fileSystem;

  // a counter used as eTag that gets incremented when any file is changed.
  unsigned int filesVersion;

  /**
   * Iterator through all Elements.
   */
  void forEach(const char *prefix, ElementCallbackFn fCallback);

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

  /**
   * Get an Element by type/name. Returns found element.
   * @param elementId type and name of element.
   * @return Element* element in list with this type/name.
   */
  Element *getElementById(const char *elementId);

  /**
   * @brief Reset/restart the board.
   * @param wipe is set to true to disconnect from WiFi and forget saved network credentials.
   */
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
  String homepage;

  // system start actions
  String sysStartAction;
  String startAction;

  // how to cache static files
  String cacheHeader;

  // short readable name of the device used for discovery and web gui
  String title;

  // short readable name of the device used for discovery and web gui
  String room;

private:
  /** Reset Counter to detect multiple hardware resets in a row. */
  int _resetCount;

  /** current state of the board. */
  enum BOARDSTATE boardState;

  /** This flag is set to true when restarting after a deep sleep allowing shortening wait times. */
  bool _isWakeupStart;

  /** if > 0; system goes to deep sleep at this millis() */
  unsigned long _deepSleepStart;

  /** if true, no deep sleep will be performed. This allows using the Web UI until next reboot. */
  bool _deepSleepBlock;

  /** time for deep sleep */
  unsigned long _deepSleepTime;


  /**
   * Add and config all Elements defined in the config files.
   */
  void _addAllElements();

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

  /**
   * Queue an action for later dispatching.
   * @param action action or property.
   * @param value the value
   */
  void _queueAction(const String &action, const String &v);

  int _addedElements = 0;

  // state and timing

  unsigned long configPhaseEnd;   // millis when current config mode (boardstate) is over, next mode
  unsigned long connectPhaseEnd;  // for waiting on net connection
  unsigned long _captiveEnd;      // terminate/reset captive portal mode after 5 minutes.
  void _newState(enum BOARDSTATE newState);

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

  /** counts loops without messages beeing passed to gracefully shut down */
  int _cntDeepSleep;

  /** list of active elements */
  Element *_elementList;

  /** next element that will be used in loop() */
  Element *_nextElement;

  String _actionList;

  static Element *_registered;
};

#endif