/**
 * @file SerialCmdElement.h
 * @brief Element reading characters from the Serial interface and executes them
 * as actions.
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
 * * 24.06.2018 created by Matthias Hertel
 * 
 * 

 "serialcmd": {
    "0": {
      "onPreset0": "",
      "onPreset1": "",
      "onPreset2": "",
      "onPreset3": "",
      "description": "Listen for commands on the Serial in line"
    }
  },

 */

#ifndef SERIALCMDELEMENT_H
#define SERIALCMDELEMENT_H

#include <Arduino.h>
#include <Element.h>

/**
 * @brief The SerialCmdElement is an special Element that reads from the serial
 * input and executes the actions.
 */
class SerialCmdElement : public Element
{
public:
  /**
   * @brief Factory function to create a SerialCmdElement.
   * @return Element*
   */
  static Element *create();


  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;


  /**
   * @brief Set a parameter or property to a new value or start an action.
   * @param name Name of property.
   * @param value Value of property.
   * @return true when property could be changed and the corresponding action
   * could be executed.
   */
  virtual bool set(const char *name, const char *value);


  /**
   * @brief Check if new characters are available and execute.
   */
  virtual void loop();

private:
  String _cmdLine;
  String _action0;
  String _action1;
  String _action2;
  String _action3;
};

#endif // SERIALCMDOTEELEMENT_H