// Actions.cpp

#include <Arduino.h>
#include <HomeDing.h>


namespace HomeDing {

namespace Action {

// this list must have all actions names inside separated and terminated by '\0'
// after adding a new value it must be lowercase and should be sorted by occurrence.
const char _s[] = {
  "value\0"
  "address\0"
  "pin\0"
  "border\0"
  "down\0"
  "height\0"
  "label\0"
  "max\0"
  "min\0"
  "onhigh\0"
  "onlow\0"
  "onvalue\0"
  "step\0"
  "text\0"
  "toggle\0"
  "type\0"
  "up\0"
  "width\0"
};


const char *find(const char *name) {
  const char *s = _s;
  const char *s_end = _s + sizeof(_s);
  size_t mem_len = strlen(name) + 1;  // len for memcmp

  while (s < s_end) {
    if (memcmp(s, name, mem_len) == 0) return (s);
    s += strlen(s) + 1;
  }
  return (nullptr);
};

// clang-format off
const char *Address  = find("address");
const char *Border   = find("border");
const char *Down     = find("down");
const char *Height   = find("height");
const char *Label    = find("label");
const char *Max      = find("max");
const char *Min      = find("min");
const char *OnHigh   = find("onhigh");
const char *OnLow    = find("onlow");
const char *OnValue  = find("onvalue");
const char *Pin      = find("pin");
const char *Step     = find("step");
const char *Text     = find("text");
const char *Toggle   = find("toggle");
const char *Type     = find("type");
const char *Up       = find("up");
const char *Value    = find("value");
const char *Width    = find("width");
// clang-format on


// pullup
// resolution
// duration
// ontemperature
// temperature
// averagetime
// filesize
// filename

// background
// brightness

// bus
// busmode

// clear
// clockpin
// datapin
// cspin
// duration
// invert
// mode
// readtime
// x
// y

};  // namespace Action
};  // namespace Homeding

// End
