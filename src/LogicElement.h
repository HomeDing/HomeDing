/**
 * @file LogicElement.h
 * @brief Logical Base Element that collects input values and settings.
 * 
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license.
 * https://www.mathertel.de/License.aspx.
 * 
 * More information on https://www.mathertel.de/Arduino
 * 
 * Changelog:
 * * 27.07.2021 created by Matthias Hertel
 */

#ifndef LOGICELEMENT_H
#define LOGICELEMENT_H

#include <HomeDing.h>

#define MAX_INPUTS 8

class LogicElement : public Element
{
public:
  /**
   * @brief Activate the Element.
   */
  virtual void start();

  /**
   * @brief Set a parameter or property to a new value or start an action.
   * @param name Name of property.
   * @param value Value of property.
   * @return true when property could be changed and the corresponding action
   * could be executed.
   */
  virtual bool set(const char *name, const char *value) override;

  /**
   * @brief push the current outgoing value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void pushState(
      std::function<void(const char *pName, const char *eValue)> callback) override;

protected:
  /// number of given input values.
  int  _inputs = 0;

  /// invert/NOT Option.
  bool _invert = false;

  /// flag set to true for the need of re-calculation of the output.
  bool _needRecalc;

  /// current input values.
  bool _value[MAX_INPUTS];

  /// current output value.
  bool _outValue;

  /// The _valueAction holds the actions that is submitted when outvalue has changed.
  String _valueAction;
};

/* ===== This Element will not be registered and configured. Use derived classes. ===== */

#endif