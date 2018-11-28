// -----
// Element.cpp - Base class for all kind of Elements.
//
// Copyright (c) by Matthias Hertel, https://www.mathertel.de.
//
// This work is licensed under a BSD style license.
// See https://www.mathertel.de/License.aspx.
// More information on https://www.mathertel.de/Arduino
// -----
// Changelog: see Element.h
// -----

#include <Board.h>
#include <Element.h>
#include <ElementRegistry.h>

/* ===== Element functions ===== */

/**
 * @brief initialize the common functionality of all element objects.
 */
void Element::init(Board *board)
{
  _board = board;
} // init()


/**
 * @brief Set a parameter or property to a new value or start an action.
 * @param name Name of property.
 * @param value Value of property.
 * @return true when property could be changed and the corresponding action
 * could be executed.
 */
bool Element::set(const char *name, const char *value)
{
  // LOGGER_ETRACE("set(%s, %s)", name, value);
  bool ret = true;

  if (_stricmp(name, "start") == 0) {
    start();
    ret = active;

  } else if (_stricmp(name, "stop") == 0) {
    term();

  } else if (_stricmp(name, "loglevel") == 0) {
    loglevel = atoi(value);

    // do not report an error for the following properties, as they are used in
    // the web ui.
  } else if (_stricmp(name, "description") == 0) {
  } else if (_stricmp(name, "room") == 0) {

  } else {
    LOGGER_EERR("cannot set property %s:", name, value);
    ret = false;
  } // if
  return (ret);
} // set()


/**
 * @brief Activate the Element.
 */
void Element::start()
{
  active = true;
} // start()


/**
 * @brief Give some processing time to the element to do something on it's own
 */
void Element::loop() {} // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void Element::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  callback("active", active ? "true" : "false");
} // pushState()


/**
 * @brief Get a property value.
 */
const char *Element::get(const char *propName)
{
  LOGGER_ETRACE("get(%s)", propName);
  String ret;

  pushState([this, propName, &ret](const char *name, const char *value) {
    // LOGGER_ETRACE("-%s:%s", name, value);
    if (_stricmp(name, propName) == 0) {
      ret = value;
    }
  });

  return (ret.c_str());
};


/**
 * @brief stop all activities and go inactive.
 */
void Element::term()
{
  LOGGER_ETRACE("term()");
  active = false;
} // term()


/* Return a boolean value from a string. */
bool Element::_atob(const char *value)
{
  bool ret = false;

  if ((!value) || (strlen(value) > 5)) {
    // ret = false;
  } else if (_stricmp(value, "1") == 0) {
    ret = true;
  } else {
    char v[8];
    strcpy(v, value);
    if (_stricmp(value, "true") == 0) {
      ret = true;
    } else if (_stricmp(value, "high") == 0) {
      ret = true;
    }
  } // if
  return (ret);
} // _atob()


/* Return a time value from a string. */
unsigned long Element::_atotime(const char *value)
{
  unsigned long ret = 0;
  unsigned long f = 1;
  char *pEnd;

  if (strchr(value, ':') != NULL) {
    // scan using format hh:mm[:ss]
    ret += strtol(value, &pEnd, 10) * 60 * 60;
    if (*pEnd == ':')
      ret += strtol(pEnd + 1, &pEnd, 10) * 60;
    if (*pEnd == ':')
      ret += strtol(pEnd + 1, &pEnd, 10);

  } else if (strchr(value, 'd') != NULL) {
    ret = strtol(value, &pEnd, 10) * 24 * 60 * 60;

  } else if (strchr(value, 'h') != NULL) {
    ret = strtol(value, &pEnd, 10) * 60 * 60;

  } else if (strchr(value, 'm') != NULL) {
    ret = strtol(value, &pEnd, 10) * 60;

  } else if (strchr(value, 's') != NULL) {
    ret = strtol(value, &pEnd, 10);

  } else {
    ret = strtol(value, &pEnd, 10);

  } // if
  return (ret);
} // _atotime()


/* Return a pin value from a string. */
int Element::_atopin(const char *value)
{
  static int GPIO[11] = {16, 5, 4, 0, 2, 14, 12, 13, 15, 3, 1};

  int pin = -1;
  if ((value) && (*value == 'D')) {
    int n = atoi(value + 1);
    if ((n >= 0) && (n <= 10))
      pin = GPIO[n];
  } else {
    pin = atoi(value);
  }
  return (pin);
} // _atopin()


int Element::_stricmp(const char *str1, const char *str2)
{
  char c1, c2;

  do {
    c1 = *str1++;
    c2 = *str2++;

    if ((c1 >= 'A') && (c1 <= 'Z'))
      c1 += 'a' - 'A';
    if ((c2 >= 'A') && (c2 <= 'Z'))
      c2 += 'a' - 'A';
  } while ((c1) && (c1 == c2));
  return (int)(c1 - c2);
} // _stricmp

// End
