
#pragma once

#include <Arduino.h>

#include <core/Logger.h>

namespace HomeDing::Actions {

/// check if the dispatch queue is empty
/// @return true when no action is in queue.
bool queueIsEmpty();


/** Queue an action for later dispatching. */
  /// @brief Queue an action for later dispatching.
  /// @param action action or property.
  /// @param value the value
  /// @param split
void push(const String &action, const char *value = nullptr, bool split = true);

/** Queue an action for later dispatching. Integer value version. */
void push(const String &action, int value, bool split = true);

/** Queue an action for later dispatching. String value version. */
void push(const String &action, const String &value, bool split = true);

String pop();

}
