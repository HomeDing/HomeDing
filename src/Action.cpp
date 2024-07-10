// Actions.cpp

#include <Arduino.h>
#include <HomeDing.h>

#include <set>

namespace HomeDing {

namespace Action {

// this list must have all actions names.
// after adding a new value it must be lowercase.
const char *_actionNames[] = {
  "value",
  "address",
  "border",
  "description",
  "pin",
  "down",
  "height",
  "label",
  "loglevel",
  "max",
  "min",
  "onhigh",
  "onlow",
  "onvalue",
  "start",
  "step",
  "stop",
  "text",
  "title",
  "toggle",
  "type",
  "up",
  "usestate",
  "startup",
  "width"
};



struct _actionless {
  bool operator()(const char *a, const char *b) const {
    return strcmp(a, b) < 0;
  }
};

std::set<const char *, _actionless> _actions(_actionNames, _actionNames + sizeof(_actionNames) / sizeof(const char *));

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
const char *Description  = find("description");
const char *Down         = find("down");
const char *Height       = find("height");
const char *Label        = find("label");
const char *LogLevel     = find("loglevel");
const char *Max          = find("max");
const char *Min          = find("min");
const char *OnHigh       = find("onhigh");
const char *OnLow        = find("onlow");
const char *OnValue      = find("onvalue");
const char *Pin          = find("pin");
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
// clang-format on

};  // namespace Action
};  // namespace Homeding

// End
