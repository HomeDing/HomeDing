/**
 * @file Actions.h
 *
 * @brief The Actions helps speeding up dispatching of Actions to Elements in the HomeDing Library.
 * Many action names are knows to be used frequently (like 'value') are looked up in the static name set.
 * This found pointer is also saved in the variables in the namespace HomeDing::Action.
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
 * see <Action.h>
 */

// Actions.cpp

#include <Arduino.h>
#include <HomeDing.h>

#include <set>

namespace HomeDing {

namespace Action {

// this list must have all actions names.
// after adding a new value it must be lowercase.
const char *_actionNames[] = {
  "address",
  "border",
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
  "onlow",
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

};  // namespace Action
};  // namespace Homeding

// End
