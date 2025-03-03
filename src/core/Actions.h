/**
 * @file Actions.h
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
 * * 22.03.2024 created by Matthias Hertel
 * * 10.07.2024 using std:set for fast finding
 * * 07.10.2024 queue of actions and dispatch functions 
*/

#pragma once

#include <Arduino.h>

#include <core/Logger.h>

namespace HomeDing::Actions {

bool _setup();

// find the action name in the Action collection or return null.
const char *find(const char *name);

// allow direct pointer compare for these actions:
extern const char *Address;
extern const char *Border;
extern const char *Brightness;
extern const char *Clear;
extern const char *Description;
extern const char *Down;
extern const char *FontSize;
extern const char *Height;
extern const char *Invert;
extern const char *Label;
extern const char *LogLevel;
extern const char *Max;
extern const char *Min;
extern const char *Mode;
extern const char *OnHigh;
extern const char *OnHumidity;
extern const char *OnLow;
extern const char *OnPressure;
extern const char *OnTemperature;
extern const char *OnValue;
extern const char *Pin;
extern const char *Redraw;
extern const char *Reference;
extern const char *Start;
extern const char *Startup;
extern const char *Step;
extern const char *Stop;
extern const char *Text;
extern const char *Title;
extern const char *Toggle;
extern const char *Type;
extern const char *Up;
extern const char *UseState;
extern const char *Value;
extern const char *Width;
extern const char *X;
extern const char *Y;


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

/** Queue an action with a value from an item in a string baseds value list. */
void pushItem(const String &action, const String &values, int n);


/// @brief get next action off the list.
/// @return next action.
String pop();

}
