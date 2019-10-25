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

#include <Arduino.h>
#include <Board.h>
#include <Element.h>

#include "SerialCmdElement.h"
#include <ElementRegistry.h>

/**
 * @brief static factory function to create a new SerialCmdElement.
 * @return SerialCmdElement* as Element* created element
 */
Element *SerialCmdElement::create()
{
  return (new SerialCmdElement());
} // create()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool SerialCmdElement::set(const char *name, const char *value)
{
  bool ret = true;

  if (_stricmp(name, "onPreset0") == 0) {
    _preset[0] = value;
  } else if (_stricmp(name, "onPreset1") == 0) {
    _preset[1] = value;
  } else if (_stricmp(name, "onPreset2") == 0) {
    _preset[2] = value;
  } else if (_stricmp(name, "onPreset3") == 0) {
    _preset[3] = value;
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

    if ((ch != '\n') && (ch != '\r') && (ch != ',')) {
      _cmdLine.concat(ch);

    } else {
      _cmdLine.toLowerCase();
      LOGGER_RAW(">%s", _cmdLine.c_str());

      if (_cmdLine == "trace") {
        // increase loggging Level
        Logger::logger_level = LOGGER_LEVEL_TRACE;
        LOGGER_RAW("Trace Level Logging enabled.");

      } else if (_cmdLine == "error") {
        // standard loggging Level
        Logger::logger_level = LOGGER_LEVEL_ERR;
        LOGGER_RAW("Error Level Logging enabled.");

      } else if (_cmdLine.startsWith("run ")) {
        int n = _cmdLine.charAt(4) - '0';
        // substitude preset values
        if ((n >= 0) && (n < 4)) {
          LOGGER_RAW("dispatch %s", _preset[n].c_str());
          _board->dispatch(_preset[n]);
        }

      } else if (_cmdLine == "elements") {
        String out;
        ElementRegistry::list(out);
        Serial.println(out);

      } else if (_cmdLine == "state") {
        String out;
        _board->getState(out, _cmdLine.substring(6));
        Serial.println(out);

      } else  if (_cmdLine.startsWith("/")) {
        _cmdLine = _cmdLine.substring(1);
        LOGGER_RAW("dispatch %s", _cmdLine.c_str());
        _board->dispatch(_cmdLine);

      } else if (_cmdLine == "help") {
        LOGGER_RAW("Commands:");
        LOGGER_RAW(" help - show all commands");
        LOGGER_RAW(" trace - enable trace logging");
        LOGGER_RAW(" error - enable error logging");
        LOGGER_RAW(" elements - list the available elements");
        LOGGER_RAW(" state - output the current state");
        LOGGER_RAW(" run {n} - execute messages preset {n}");
        LOGGER_RAW(" /{type}/{id}?{property}={value} - dispatch it");

      } else {
        LOGGER_RAW("dispatch %s", _cmdLine.c_str());
        _board->dispatch(_cmdLine);
      }
      _cmdLine = "";
    } // if
  } // while
} // loop()


// End
