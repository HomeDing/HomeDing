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

#include <Arduino.h>
#include <HomeDing.h>

#define TRACE(...)  // LOGGER_ETRACE(__VA_ARGS__)

/* ===== Element functions ===== */

/**
 * @brief initialize the common functionality of all element objects.
 */
void Element::init(Board *board) {
  _board = board;
}  // init()


/**
 * @brief Set a parameter or property to a new value or start an action.
 * @param name Name of property.
 * @param value Value of property.
 * @return true when property could be changed and the corresponding action
 * could be executed.
 */
bool Element::set(const char *name, const char *value) {
  // TRACE("set %s=%s", name, value);
  bool ret = true;

  if (_stricmp(name, "start") == 0) {
    start();
    ret = active;

  } else if (_stricmp(name, "stop") == 0) {
    term();

  } else if (_stricmp(name, "logLevel") == 0) {
    loglevel = _atoi(value);

  } else if (_stricmp(name, "useState") == 0) {
    _useState = _atob(value);

    // do not report an error for the following properties,
    // as they are used by the web ui and stored in the config files only.
  } else if (_stricmp(name, "description") == 0) {
  } else if (_stricmp(name, "title") == 0) {

  } else {
    // LOGGER_EERR("cannot set property %s:", name, value); // not an error when used for testing common properties
    ret = false;
  }  // if
  return (ret);
}  // set()


/**
 * @brief setup the element so it can be started and stopped.
 */
void Element::setup() {}


/**
 * @brief Activate the Element.
 */
void Element::start() {
  active = true;
}  // start()


/**
 * @brief Give some processing time to the element to do something on it's own
 */
void Element::loop() {}  // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void Element::pushState(
  std::function<void(const char *pName, const char *eValue)> callback) {
  callback("active", active ? "true" : "false");
}  // pushState()



/// @brief save a local state to a state element.
/// @param key The key of state variable.
/// @param value The value of state variable.
void Element::saveState(const char *key, String value) {
  saveState(key, value.c_str());
}


/// @brief save a local state to a state element.
/// @param key The key of state variable.
/// @param value The value of state variable.
void Element::saveState(const char *key, const char *value) {
  TRACE("saveState(%s=%s)", key, value);
  if (active && _useState) {
    DeviceState::setElementState(this, key, value);
  }
}  // saveState


/**
 * @brief stop all activities and go inactive.
 */
void Element::term() {
  TRACE("term()");
  active = false;
}  // term()

// ===== static string to value helper function ===== //

/* Return am integer value from a string. */
int Element::_atoi(const char *value) {
  return (strtol(value, nullptr, 0));
}  // _atoi()


/* Return a boolean value from a string. */
bool Element::_atob(const char *value) {
  bool ret = false;

  if ((!value) || (strnlen(value, 6) > 5)) {
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
  }  // if
  return (ret);
}  // _atob()


/* Return a time value from a string. */
unsigned long Element::_atotime(const char *value) {
  unsigned long ret = 0;
  char *pEnd;

  if (strchr(value, ':') != NULL) {
    // scan using format hh:mm[:ss]
    ret += strtol(value, &pEnd, 10) * 60 * 60;
    if (*pEnd == ':')
      ret += strtol(pEnd + 1, &pEnd, 10) * 60;
    if (*pEnd == ':')
      ret += strtol(pEnd + 1, &pEnd, 10);

  } else {
    ret = strtol(value, &pEnd, 10);
    if (*pEnd == 'd') {
      ret *= (24 * 60 * 60);
    }
    if (*pEnd == 'h') {
      ret *= (60 * 60);
    }
    if (*pEnd == 'm') {
      ret *= (60);
    }
    // if (*pEnd == 's') { ret *= 1 }
  }  // if
  return (ret);
}  // _atotime()


/* Return a duration value from a string as milliseconds. */
unsigned long Element::_scanDuration(const char *value) {
  unsigned long ret = 0;
  char *pEnd;

  if (strchr(value, ':') != nullptr) {
    // scan using format hh:mm[:ss[.mmm]]
    ret += strtol(value, &pEnd, 10) * 60 * 60 * 1000;
    if (*pEnd == ':')
      ret += strtol(pEnd + 1, &pEnd, 10) * 60 * 1000;
    if (*pEnd == ':') {
      ret += strtol(pEnd + 1, &pEnd, 10) * 1000;
      if (*pEnd == '.') {
        char *pMS;
        pEnd++;
        unsigned long ms = strtol(pEnd, &pMS, 10);
        while (pMS - pEnd < 3) {
          ms = 10 * ms;
          pMS++;
        }
        ret += ms;
      }
    }

  } else if (strstr(value, "ms") != nullptr) {
    // scan using format 000ms
    ret = strtol(value, &pEnd, 10);

  } else {
    ret = strtol(value, &pEnd, 10);
    if (*pEnd == 'd') {
      ret *= (24 * 60 * 60 * 1000);
    } else if (*pEnd == 'h') {
      ret *= (60 * 60 * 1000);
    } else if (*pEnd == 'm') {
      ret *= (60 * 1000);
    } else {  // simple seconds
      ret *= 1000;
    }
  }  // if
  return (ret);
}  // _scanDuration()


bool Element::_scanIndexParam(const char *name, size_t &index, String &indexName) {
  const char *p = name;

  p = strchr(p, '[');
  if (p) {
    index = (size_t)strtoul(p + 1, nullptr, 10);
    p = strchr(p, ']');
  }
  if (p) {
    p = strchr(p, '/');
  }
  if (p) {
    indexName = p + 1;
  }
  return (p);
}  // _scanIndexParam()


/* Return a pin value from a string. */
int Element::_atopin(const char *value) {
#if defined(ESP8266)
  static int GPIO[11] = { 16, 5, 4, 0, 2, 14, 12, 13, 15, 3, 1 };
#endif

  int pin = -1;
  if (value) {
#if defined(ESP8266)
    char ch = tolower(*value);
    if (ch == 'd') {
      int n = _atoi(value + 1);  // scan a number right after the 'D'
      if ((n >= 0) && (n <= 10))
        pin = GPIO[n];

    } else if (ch == 'a') {
      pin = A0;  // only analog pin on ESP8266

    } else {
      pin = _atoi(value);
    }
#else
    pin = _atoi(value);
#endif
  }
  return (pin);
}  // _atopin()


uint32_t Element::_atoColor(const char *value) {
  uint32_t ret = 0x00000000;  // black

  if (value) {
    char ch0 = value[0];

    if ((ch0 == '#') || (ch0 == 'x')) {
      ret = strtoul(value + 1, nullptr, 16);
    } else if ((ch0 >= '0') && (ch0 <= '9')) {
      ret = _atoi(value);
    } else if (_stricmp(value, "black") == 0) {
      ret = 0x00000000;
    } else if (_stricmp(value, "red") == 0) {
      ret = 0x00FF0000;
    } else if (_stricmp(value, "green") == 0) {
      ret = 0x0000FF00;
    } else if (_stricmp(value, "blue") == 0) {
      ret = 0x000000FF;
    } else if (_stricmp(value, "white") == 0) {
      ret = 0xFFFFFFFF;
    }
  }  // if
  return ret;
}  // _atoColor()


// ===== static value to string helper function ===== //

char Element::_convertBuffer[32];

char *Element::_printBoolean(bool b) {
  return (char *)(b ? "1" : "0");
};

char *Element::_printInteger(int v) {
  itoa(v, _convertBuffer, 10);
  return (_convertBuffer);
};

char *Element::_printInteger(unsigned long v) {
  ultoa(v, _convertBuffer, 10);
  return (_convertBuffer);
};

// ===== static general string helper function ===== //

int Element::_stricmp(const char *str1, const char *str2) {
  char c1, c2;

  do {
    c1 = *str1++;
    c2 = *str2++;

    // convert characters from upper to to lowercase
    if ((c1 >= 'A') && (c1 <= 'Z'))
      c1 += 'a' - 'A';
    if ((c2 >= 'A') && (c2 <= 'Z'))
      c2 += 'a' - 'A';
  } while ((c1) && (c1 == c2));
  return (int)(c1 - c2);
}  // _stricmp


// String start with prefix, case insensitive.
bool Element::_stristartswith(const char *s, const char *prefix) {
  bool ret = true;  // until we find a difference
  if (s && prefix) {
    while (ret && *s && *prefix) {
      if (tolower(*s) != tolower(*prefix))
        ret = false;
      s++;
      prefix++;
    }  // while
    if (*prefix)
      ret = false;
  } else {
    ret = false;
  }
  return (ret);
}  // _stristartswith()


void Element::_strlower(char *str) {
  if (str) {
    while (*str) {
      if ((*str >= 'A') && (*str <= 'Z'))
        *str += 'a' - 'A';
      str++;
    }  // while
  }    // if
}  // _strlower


/** Get item[index] from string */
String Element::getItemValue(String data, int index) {
  String ret;
  int found = 0;
  int startIndex = 0;
  int endIndex = -1;

  int i = 0;
  const char *p = data.c_str();

  while (found <= index) {
    if ((*p == LIST_SEPARATOR) || (!*p)) {
      found++;
      startIndex = endIndex + 1;
      endIndex = i;
    }
    if (!*p) {
      break;
    }
    i++;
    p++;
  }  // while

  if (found > index) {
    ret = data.substring(startIndex, endIndex);
  }

  return (ret);
}  // getItemValue()


/** Get first item from string and remove from string */
String Element::popItemValue(String &data) {
  String item;

  if (data.length() > 0) {
    // extract first action
    int pos = data.indexOf(LIST_SEPARATOR);
    if (pos > 0) {
      item = data.substring(0, pos);
      data.remove(0, pos + 1);
    } else {
      item = data;
      data = (const char *)nullptr;
    }
  }  // if
  return (item);
}  // popItemValue


// End
