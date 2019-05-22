/**
 * @file MenuElement.h
 * @brief Menu Element to control multiple values by a rotary encoder.
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
 * * 30.05.2019 created by Matthias Hertel
 */

#ifndef MENUELEMENT_H
#define MENUELEMENT_H

#define MAXMENUVALUES 8

class MenuElement : public Element
{
public:
  /**
   * @brief Factory function to create a MenuElement.
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
   * @brief Activate the Element.
   * @return true when the Element could be activated.
   * @return false when parameters are not usable.
   */
  virtual void start();

  /**
   * @brief Give some processing time to the timer to check for next action.
   */
  virtual void loop();

  /**
   * @brief push the current value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void pushState(
      std::function<void(const char *pName, const char *eValue)> callback);

private:
  /**
   * @brief The actual value.
   */
  int _value;

  /** The actual active visualized and changeable value */
  int _active = 0;

  /** The number of value elements in the list. */
  int _count = 0;

ValueElement *valueList[MAXMENUVALUES];

  /** Flag for need to update the value. */
  bool _updateV = false;
  bool _updateM = false;


  /**
   * @brief The _xAction holds the actions that is submitted when ...
   */
  String _displayAction;
  String _valueAction;
  String _menuAction;


};

#endif