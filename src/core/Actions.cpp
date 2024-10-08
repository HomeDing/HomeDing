
#include <core/Actions.h>

#include <ListUtils.h>
#include <ArrayString.h>

namespace HomeDing::Actions {

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


String pop() {
  return actions.pop();
};


}
