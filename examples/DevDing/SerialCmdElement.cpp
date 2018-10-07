/**
 * @file SerialCmdElement.cpp
 * 
 * @brief Local Element for the HomeDing Library that listens to input on the serial interface.
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
 * Changelog: see SerialCmdElement.h
 */

#include "SerialCmdElement.h"
#include "ElementRegistry.h"

#undef LOGGER_MODULE
#define LOGGER_MODULE "cmd"
#include "core/Logger.h"


/**
 * @brief static factory function to create a new SerialCmdElement.
 * @return SerialCmdElement* as Element* created element
 */
Element *SerialCmdElement::create()
{
  LOGGER_TRACE("create()");
  return (new SerialCmdElement());
} // create()


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
    // LOGGER_INFO(">>%c", ch);

    if ((ch == '\n') || (ch == '\r') || (ch == ',')) {
      if (_cmdLine == "+") {
        // increase loggging Level
        LOGGER_INFO("Trace Level Logging enabled.");
        Logger::logger_level = LOGGER_LEVEL_TRACE;

      } else if (_cmdLine == "-") {
        // standard loggging Level
        LOGGER_INFO("Error Level Logging enabled.");
        Logger::logger_level = LOGGER_LEVEL_ERR;

      } else if (_cmdLine.length() == 1) {
        // substitude preset values
        if (_cmdLine.charAt(0) == '0')
          _cmdLine = _preset0;
        else if (_cmdLine.charAt(0) == '1')
          _cmdLine = _preset1;

      } else {
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
