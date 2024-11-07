/**
 * @file Actions.cpp
 *
 * @brief The Actions helps speeding up dispatching of Actions to Elements in the HomeDing Library.
 * Many action names are knows to be used frequently (like 'value') are looked up in the static name set.
 * This found pointer is also saved in the variables in the namespace HomeDing::Actions.
 * This allows comparing pointers instead of the high resource consuming string comparisation
 * in the set() functions of the Elements.
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
 * see <core/Actions.h>
 */

#include <core/Actions.h>
#include <HomeDing.h>

#include <set>

#include <ListUtils.h>
#include <ArrayString.h>

namespace HomeDing::Actions {

// this list must have all actions names.
// after adding a new value it must be lowercase.
const char *_actionNames[] = {
  "address",
  "border",
  "brightness",
  "clear",
  "description",
  "down",
  "height",
  "invert",
  "label",
  "loglevel",
  "max",
  "min",
  "mode",
  "onhigh",
  "onhumidity",
  "onlow",
  "onpressure",
  "ontemperature",
  "onvalue",
  "pin",
  "redraw",
  "reference",
  "start",
  "startup",
  "step",
  "stop",
  "text",
  "title",
  "toggle",
  "type",
  "up",
  "usestate",
  "value",
  "width",
  "x",
  "y"
};


std::set<const char *, _pchar_less> _actions(_actionNames, _actionNames + sizeof(_actionNames) / sizeof(const char *));

const char *find(const char *name) {
  auto it = _actions.find(name);

  if (it == _actions.end()) {
    return (nullptr);
  }
  return (*it);
}


// clang-format off
const char *Address      = find("address");
const char *Border       = find("border");
const char *Brightness   = find("brightness");
const char *Clear        = find("clear");
const char *Description  = find("description");
const char *Down         = find("down");
const char *Height       = find("height");
const char *Invert       = find("invert");
const char *Label        = find("label");
const char *LogLevel     = find("loglevel");
const char *Max          = find("max");
const char *Min          = find("min");
const char *Mode         = find("mode");
const char *OnHigh       = find("onhigh");
const char *OnLow        = find("onlow");
const char *OnValue      = find("onvalue");
const char *Pin          = find("pin");
const char *Redraw       = find("redraw");
const char *Reference    = find("reference");
const char *Start        = find("start");
const char *Startup      = find("startup");
const char *Step         = find("step");
const char *Stop         = find("stop");
const char *Text         = find("text");
const char *Title        = find("title");
const char *Toggle       = find("toggle");
const char *Type         = find("type");
const char *Up           = find("up");
const char *UseState     = find("usestate");
const char *Value        = find("value");
const char *Width        = find("width");
const char *X            = find("x");
const char *Y            = find("y");
// clang-format on


// ===== Queue =====
ArrayString actions;

bool queueIsEmpty() {
  return (actions.empty());
}


/** Queue an action for later dispatching. */
void push(const String &action, int value, bool split) {
  if (!action.isEmpty()) {
    char _convertBuffer[16];
    itoa(value, _convertBuffer, 10);
    push(action, _convertBuffer, split);
  }
}  // push


void push(const String &action, const String &value, bool split) {
  if (!action.isEmpty()) {
    push(action, value.c_str(), split);
  }
}  // push


/** Queue an action for later dispatching. */
void push(const String &action, const char *value, bool split) {
  if (!action.isEmpty()) {
    String tmp = action;
    if (value) {
      tmp.replace("$v", value);
    }

    // #if defined(LOGGER_ENABLED)
    //     if (Logger::logger_level >= LOGGER_LEVEL_TRACE) {
    //       Logger::printf("#action (%s)=>%s", (_activeElement ? _activeElement->id : ""), tmp.c_str());
    //     }
    // #endif

    if (split) {
      int len = ListUtils::length(tmp);
      for (int n = 0; n < len; n++) {
        actions.push(ListUtils::at(tmp, n).c_str());
      }
    } else {
      actions.push(tmp);
    }
  }
}  // push


/** Queue an action with a value from an item in a string baseds value list. */
void pushItem(const String &action, const String &values, int n) {
  if (action && values) {
    String v = Element::getItemValue(values, n);
    if (v) {
      push(action, v.c_str());
    }
  }  // if
}  // pushItem


String pop() {
  return actions.pop();
};


}
