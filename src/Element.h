// -----
// Element.h - Base class for actions, timer, ...
//
// Copyright (c) by Matthias Hertel, https://www.mathertel.de.
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

#include "Board.h"
#include <Arduino.h>

#define ACTION_SEPARATOR ','

// forward class declarations
class Board;

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
   * @brief The Element will be marked active after passing valid parameters and
   * calling start().
   */
  bool active = false;


protected:
  /**
   * @brief Return a boolean value from a string.
   * @param value Given value as string.
   * @return true Return for "true", "on", "1", "high".
   * @return false Return false as the default case.
   */
  bool _atob(const char *value);

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
   *
   * @param value Given value as string.
   * @return time or duration as seconds.
   */
  unsigned long _atotime(const char *value);

  /**
   * @brief replacement of the CPP stricmp function not available on Arduino.
   */
  int _stricmp(const char *str1, const char *str2);

  /**
   * @brief A reference to the board the Element is on.
   */
  Board *_board;

  // private:
};


#include "ElementRegistry.h"

// ===== CORE Elements =====

// Elements that are required in any case add themselves to the ElementRegistry
// See SSDPElement.cpp or DeviceElement.cpp as examples.

// easy include of all libraries that do not require a separate library
#ifdef HOMEDING_INCLUDE_CORE

#define HOMEDING_INCLUDE_Button
#define HOMEDING_INCLUDE_Timer


#endif

#ifdef HOMEDING_INCLUDE_Button
#include <ButtonElement.h>
bool ButtonElement::registered =
    ElementRegistry::registerElement("button", ButtonElement::create);
#endif

#ifdef HOMEDING_INCLUDE_Timer
#include <TimerElement.h>
bool TimerElement::registered =
    ElementRegistry::registerElement("timer", TimerElement::create);
#endif

#ifdef HOMEDING_INCLUDE_DigitalOut
#include <DigitalOutElement.h>
bool DigitalOutElement::registered =
    ElementRegistry::registerElement("digitalout", DigitalOutElement::create);
#endif


// ===== OPTIONAL ELEMENTS =====

#ifdef HOMEDING_INCLUDE_DHT
#include <DHTElement.h>
bool DHTElement::registered =
    ElementRegistry::registerElement("dht", DHTElement::create);
#endif


#endif