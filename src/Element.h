/**
 * @file Element.h
 * @brief Base Element class of the HomeDing Library implementing the default
 * functionality. All Elements must derive from this class.
 * @author Matthias Hertel, https://www.mathertel.de
 */
//
// This work is licensed under a BSD style license.
// See https://www.mathertel.de/License.aspx.
// More information on https://www.mathertel.de/Arduino
// -----
// 23.04.2018 created by Matthias Hertel
// 27.04.2018 parameter pushing & loading added.
// 29.04.2018 action passing added.
// 15.05.2018 set = properties and action interface.
// 15.02.2024 CATEGORY added.
// -----

#pragma once

#include <cstdarg>
#include <functional>
#include <memory>
#include <string.h>

#include <ListUtils.h>
#include <ArrayString.h>

// #include <hdProfile.h>

// forward class declarations
class Board;
class Element;

// enable / disable core features

// ===== Pins =====
// some EPS8266 boards do not have Dx pins defined.+
// Here fore compatibility reasons...
#ifndef D3
#define D3 0
#endif

#ifndef D4
#define D4 2
#endif

// ===== Helping classes =====
#include <core/Logger.h>

/**
 * @brief Startup Mode specifies when is the right moment to try
 * starting/activating the element.
 */
enum Element_StartupMode : uint16_t {
  System = 1,       // right after loading the configurations.
  WithNetwork = 2,  // after a network connectivity in AP Mode was established.
  WithTime = 3,     // after a valid local time was set.
  Manual = 9        // manually started.
};

#define ACTION_SEPARATOR ','

#define VALUE_SEPARATOR ','
#define LIST_SEPARATOR ','

// id can be multi-level when using the slash as a separator.
// like "device/name"
#define MAX_ID_LENGTH 32
#define ELEM_ID_SEPARATOR '/'
#define ELEM_PARAMETER '?'
#define ELEM_VALUE '='


/**
 * @brief This is the base class for all Elements that can be managed by the
 * Board.
 */
class Element {
public:
  /// @brief Datatype definitions for elements processing different types based on the configuration.
  enum DATATYPE : uint16_t {
    STRING = 0,  // unspecified, all data can be presented as strings
    BOOLEAN,     // a boolean (stored as integer 0/1)
    INTEGER,     // a number without any decimals
    FLOAT        // a number with decimals
  };

  /// @brief CATEGORY definitions for elements. Features supported by the Element or Element base class.
  enum CATEGORY : uint16_t {
    Looping = 0x01,   // using the loop() function.
    Standard = 0x10,  // is a normal Element
    Display = 0x20,   // is a DisplayElement
    Widget = 0x40,    // is a DisplayOutputElement
    All = 0xff        // all elements
  };

  /// @brief The id of the Element.
  char id[MAX_ID_LENGTH];  // TODO: convert to String

  /// @brief The detail of logging for this Element.
  uint16_t loglevel = LOGGER_LEVEL_ERR;

  /// @brief The default features for this Element.
  CATEGORY category = (CATEGORY)(CATEGORY::Standard | CATEGORY::Looping);

#if defined(HD_PROFILE)
  PROFILE_DATA;
#endif

  /// @brief The Element will be marked active after passing valid parameters and
  /// calling start(). Then loop() function will be called periodically.
  ///
  bool active = false;


  /**
   * @brief when the element should be started.
   */
  Element_StartupMode startupMode = Element_StartupMode::WithNetwork;


  /**
   * @brief The pointer to the next Element in the list of all Elements.
   */
  Element *next = NULL;


  // ===== Livetime management =====

  /// @brief initialize a new Element.
  /// @param board The board reference.
  virtual void init(Board *board);


  /// @brief Set a parameter or property to a new value or start an action.
  /// @param name Name of the property.
  /// @param value Value of the property.
  /// @return true when property was set or the action was received.
  virtual bool set(const char *name, const char *value);


  /// @brief setup the element so it can be started and stopped.
  virtual void setup();

  //// @brief Activate the Element.
  virtual void start();


  /**
   * @brief Give some processing time to the element to do something on it's own
   */
  virtual void loop();


  /**
   * @brief stop all activities and go inactive.
   */
  virtual void term();


  /// @brief push the current value of all properties to the callback.
  /// @param callback callback function that is used for every property.
  virtual void pushState(
    std::function<void(const char *pName, const char *eValue)> callback);


  /// @brief save a local state to a state element.
  /// @param key The key of state variable.
  /// @param value The value of state variable.
  void saveState(const char *key, String value);

  /// @brief save a local state to a state element.
  /// @param key The key of state variable.
  /// @param value The value of state variable.
  void saveState(const char *key, const char *value);


  // ===== static string to value helper function ===== //

  /**
   * @brief Return an integer value from a string in various formats.
   * @param value Given value as string.
   * @return value from string.
   */
  static int _atoi(const char *value);

  /**
   * @brief Return a boolean value from a string.
   * @param value Given value as string.
   * @return Return true for `1`, `true`, `high` or `on`. Return false otherwise.
   */
  static bool _atob(const char *value);

  /**
   * @brief Return a time value from a string.
   * @details Time values as represented using a long integer containing the
   * time / duration as seconds.
   *
   * A unit modifier can be appended:
   * * "d" for days
   * * "h" for hours
   * * "m" for minutes
   * * "s" for seconds
   * The format hh:mm[:ss] can also be used.
   *
   * @param value Given value as string.
   * @return time or duration as seconds.
   */
  static unsigned long _atotime(const char *value);

  /**
   * @brief Return a duration value from a string as milliseconds.
   * @param value Given value as string.
   * @return time or duration as milliseconds.
   */
  static unsigned long _scanDuration(const char *value);

  /**
   * @brief Return a pin value from a string.
   * @details pin values can be entered using the "D0" or "A0" syntax or by
   * specifying a GPIO number. Mappings are taken from NodeMCU.
   *
   * * D0  = GPIO 16
   * * D1  = GPIO 5
   * * D2  = GPIO 4
   * * D3  = GPIO 0
   * * D4  = GPIO 2
   * * D5  = GPIO 14
   * * D6  = GPIO 12
   * * D7  = GPIO 13
   * * D8  = GPIO 15
   * * D9  = GPIO 3
   * * D10 = GPIO 1
   *
   * @param value Given value as string.
   * @return GPIO Number.
   */
  static int _atopin(const char *value);


  /// @brief Return a color value as 32 bits: 0xWWRRGGBB from a string.
  /// @param value Given value as string.
  /// @return color value
  uint32_t _atoColor(const char *value);


  /**
   * @brief extract index and property name from a configuration string
   * with arrays like list[3]/property
   * @param name Given indexed configuration "name" like "list[3]/property".
   * @return index Reference to index variable, will contain the index (here 3)
   * @return indexName Reference to indexName variable, will contain the index name (here "property")
   * @return true, when scanning was complete
   */
  bool _scanIndexParam(const char *name, size_t &index, String &indexName);

  // ===== static value to string helper function ===== //

  static char *_printBoolean(bool b);
  static char *_printInteger(int v);
  static char *_printInteger(unsigned long v);


  // ===== static general string helper function ===== //

  /**
   * @brief replacement of the CPP stricmp function not available on Arduino.
   */
  static int _stricmp(const char *str1, const char *str2);


  /// @brief String start with prefix, case sensitive.
  static bool _strStartsWith(const char *s, const char *prefix);


  /**
   * @brief String start with prefix, case insensitive.
   */
  static bool _stristartswith(const char *s, const char *prefix);


  /**
   * @brief to lowercase a c string.
   */
  static void _strlower(char *str);


  /* ===== String as List functions =====*/
  // These are useful function to use a String
  // as a List of strings separated by LIST_SEPARATOR (',').

  /** Get item[index] from string */
  static String getItemValue(String data, int index);

  /** Get first item from string and remove from string */
  static String popItemValue(String &data);


protected:
  /// @brief A reference to the board the Element is on.
  Board *_board;

  /**
   * @brief Flag to mark that the element should save the state.
   */
  bool _useState = false;

private:
  static char _convertBuffer[32];
};

// End
