/**
 * @file Board.h
 * 
 * @brief Inferface of the Board class for the HomeDing Library.
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
 */

// The Board.h file also works as the base import file that contains some
// general definitions.

#ifndef BOARD_H
#define BOARD_H

// Debug output using Serial

#ifdef DEBUG_ESP_PORT
#define DEBUG_MSG(...) DEBUG_ESP_PORT.printf(__VA_ARGS__)
#else
#define DEBUG_MSG(...)
#endif

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <memory>
#include <time.h>

#include <DisplayAdapter.h>

// id can be multi-level when using the slash as a separator.
// like "device/name"
#define ELEM_ID_SEPARATOR '/'
#define ELEM_PARAMETER '?'
#define ELEM_VALUE '='

/**
 * @brief The confnet.json file contains all the settings for registering the device on the network. 
  */
#define CONFNET_FILENAME "/confnet.json"

/**
 * @brief The config.json file contains all the settings for the device elements. 
 */
#define CONF_FILENAME "/config.json"

#define MAX_ID_LENGTH 32

#define MAX_LIST_LENGTH 20

// forward class declarations
class Element;


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
  /**
   * @brief initialize a blank board.
   * @param d DisplayAdapter for an existing display
   * @param s WebServer
   */
  void init(DisplayAdapter *d = NULL, ESP8266WebServer *s = NULL);

  // ElementRegistry
  // allow registration of Element Types to avoid hard references.
  // static bool registerElement(const char *elementTypeName,
  //                      std::unique_ptr<Element *()> factoryFunc);

  /**
   * @brief Add and config the Elements defined in the config file
   */
  void addElements();

  /**
   * @brief activate all the Elements by using start().
   */
  void start();

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
  void getState(String &out, String path);


  /*
   * @brief Set a single property to a specific value or start an action.
   * @param path Path of an Element.
   * @param property Name of the property
   * @param value New value of the property.
   */
  void setState(String &path, String property, String value);

  // Time functionality

  // fill the time structure from a timestamp;
  void getTime(struct tm *time);

  // return the seconds of today.
  time_t getTimeOfDay();


  // Display
  DisplayAdapter *display = NULL;

  // WebServer
  ESP8266WebServer *server;

  /**
   * @brief Get a Element by typename. Returns the first found element.
   * This method can be used to access the singletopn Elements like device,
   * displa, ota, ...
   * @param elementTypeName type name of element.
   * @return Element* first element in list with this type.
   */
  Element *getElement(const char *elementTypeName);

private:
  /**
   * @brief Add a new element
   * @param id
   * @param e
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

  bool active = false;

  Element *_list2;
  Element *_next2;

  String _actionList;

  static Element *_registered;
};

#endif