/**
 * @file DigitalOutElement.h
 * @brief Output Element of the HomeDing Library to output an digital signals based on actions.
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
 * * 29.04.2018 created by Matthias Hertel
 */

#ifndef DIGITALOUT_H
#define DIGITALOUT_H

#include <Arduino.h>

#include "Board.h"
#include "Element.h"

/**
 * @brief The DigitalOutElement is an Element that creates an digital output
 * signal based on actions.
 */
class DigitalOutElement : public Element
{
public:
  /**
   * @brief Factory function to create a DigitalOutElement.
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
   * @brief Activate the DigitalOutElement.
   * @return true when activation was good.
   * @return false when activation failed.
   */
  virtual void start();

  virtual void pushState(
      std::function<void(const char *pName, const char *eValue)> callback);

private:
  int _pin = -1;

  // when _inverse == true: logical HIGH will produce a physical LOW level.
  bool _invers = false;

  // The last logical level LOW or HIGH
  int _lastLevel; //

  void _setLevel(bool logicalHigh); // set the physical level based on _inverse
};

#endif // DIGITALOUT_H