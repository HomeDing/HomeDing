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
// -----

#ifndef ELEMENT_H
#define ELEMENT_H

#include <string.h>
#include <functional>
#include <memory>
#include <cstdarg> 

// forward class declarations
class Board;
class Element;


// ===== Pins =====
// some EPS8266 boards do not have Dx pins defined.+
// Here fore compatibility reasons...
#ifndef D3
#define D3 0
#endif

#ifndef D4
#define D4 2
#endif

#define UNUSED __attribute__ ((unused)) 

// ===== Helping classes =====
#include <core/Logger.h>

/**
 * @brief Startup Mode specifies when is the right moment to try
 * starting/activating the element.
 */
enum Element_StartupMode {
  System = 1, // right after loading the configurations.
  Network = 2, // after a network connectivity in AP Mode was established.
  Time = 3, // after a valid local time was set.
  Manual = 9 // manually started.
};

#define ACTION_SEPARATOR ','

#define VALUE_SEPARATOR ','

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
class Element
{
public:
  /**
   * @brief The id of the Element. Visible to anyone.
   */
  char id[MAX_ID_LENGTH]; // TODO: convert to String

  int loglevel = LOGGER_LEVEL_ERR;

  /**
   * @brief The Element will be marked active after passing valid parameters and
   * calling start().
   */
  bool active = false;


  /**
   * @brief when the element should be started.
   */
  Element_StartupMode startupMode = Element_StartupMode::Network;


  /**
   * @brief The pointer to the next Element in the list of all Elements.
   */
  Element *next = NULL;


  // ===== Livetime management =====

  /**
   * @brief initialize a new Element.
   * @param board The board reference.
   */
  virtual void init(Board *board);


  /**
   * @brief Set a parameter or property to a new value or start an action.
   * @param name Name of property.
   * @param value Value of property.
   * @return true when property could be changed and the corresponding action
   * could be executed.
   */
  virtual bool set(const char *name, const char *value);


  /**
   * @brief Get a property value.
   * @param name Name of property.
   * @return actual value of property.
   */
  virtual const char *get(const char *name);


  /**
   * @brief Activate the Element.
   * @return true when activation was good.
   * @return false when activation failed.
   */
  virtual void start();


  /**
   * @brief Give some processing time to the element to do something on it's own
   */
  virtual void loop();


  /**
   * @brief stop all activities and go inactive.
   */
  virtual void term();


  /**
   * @brief push the current value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void pushState(
      std::function<void(const char *pName, const char *eValue)> callback);

  /**
   * @brief Return an integer value from a string in various formats.
   * @param value Given value as string.
   * @return value from string.
   */
  static int _atoi(const char *value);

  /**
   * @brief Return a boolean value from a string.
   * @param value Given value as string.
   * @return true Return for "true", "on", "1", "high".
   * @return false Return false as the default case.
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


  /**
   * @brief Return a color value as 32 bits: 0xWWRRGGBB from a string.
   * @param value Given value as string.
   * @return color value
   */
  uint32_t _atoColor(const char *value);


  /**
   * @brief replacement of the CPP stricmp function not available on Arduino.
   */
  static int _stricmp(const char *str1, const char *str2);

  /**
   * @brief to lowercase a c string.
   */
  static void _strlower(char *str);


/** Get item[index] from string */
static String getItemValue(String data, int index);


protected:
  /**
   * @brief A reference to the board the Element is on.
   */
  Board *_board;

  // private:
};

#endif