/**
 * @file SerialCmdElement.cpp
 * @brief Input Element for the IoT Board Library typicaly used with momentary
 * buttons or switches.
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license,
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino.
 *
 * Changelog: see SerialCmdElement.h
 */

#include "SerialCmdElement.h"
#include "ElementRegistry.h"

#define LOGGER_MODULE "cmd"
#include "Logger.h"


/**
 * @brief static factory function to create a new SerialCmdElement.
 * @return SerialCmdElement* as Element* created element
 */
Element *SerialCmdElement::create()
{
  LOGGER_TRACE("create()");
  return (new SerialCmdElement());
} // create()


SerialCmdElement::SerialCmdElement()
{
  // set some defaults
  _cmdLine.reserve(40);
} // SerialCmdElement()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool SerialCmdElement::set(const char *name, const char *value)
{
  LOGGER_TRACE("set(%s:%s)", name, value);
  bool ret = true;

  if (_stricmp(name, "preset0") == 0) {
    _preset0 = value;
  } else if (_stricmp(name, "preset1") == 0) {
    _preset1 = value;
  } else {
    ret = Element::set(name, value);
  } // if
  return (ret);
} // set()


/**
 * @brief Check if new characters are available and execute.
 */
void SerialCmdElement::loop()
{
  while (Serial.available()) {
    char ch = Serial.read();
    LOGGER_INFO(">>%c", ch);
    if ((ch == '\n') || (ch == '\r') || (ch == ',')) {

      if (_cmdLine.length() == 1) {
        // substitude preset values
        if (_cmdLine.charAt(0) == '0')
          _cmdLine = _preset0;
        else if (_cmdLine.charAt(0) == '1')
          _cmdLine = _preset1;
      }

      if (_cmdLine.length() > 1) {
        LOGGER_INFO("dispatch %s", _cmdLine.c_str());
        _board->dispatch(_cmdLine);
        _cmdLine = "";
      }
    } else {
      _cmdLine.concat(ch);
    } // if
  } // while
} // loop()


// Register the SerialCmdElement in the ElementRegistry.
bool SerialCmdElement::registered =
    ElementRegistry::registerElement("serialcmd", SerialCmdElement::create);

// End
