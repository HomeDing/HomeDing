/**
 * @file CalcElement.h
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
 * * 10.10.2021 created by Matthias Hertel
 */

#pragma once

#define CALCELEMENT_MAX_INPUTS 8

class CalcElement : public Element {
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
   * @brief Give some processing time to the element to do something on it's own
   */
  virtual void loop();


  /**
   * @brief push the current outgoing value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void pushState(
    std::function<void(const char *pName, const char *eValue)> callback) override;

protected:
  /// number of given input values.
  int _inputs = 0;

  Element::DATATYPE _type = DATATYPE::STRING;

  /// invert/NOT Option for boolean outputs.
  bool _invert = false;

  /// flag set to true for the need of re-calculation of the output.
  bool _needRecalc;

  /// current string based input values.
  String _inStringValues[CALCELEMENT_MAX_INPUTS];

  /// current (formatted) output value.
  String _value;

  /// The _valueAction holds the actions that is submitted when outvalue has changed.
  String _valueAction;

  /**
   * @brief function for calculating from input to output values.
   */
  virtual void _calc();
};

/* ===== This Element will not be registered and configured. Use derived classes. ===== */
