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
 * * 06.12.2020 some rework and doku.
 */

#ifndef MENUELEMENT_H
#define MENUELEMENT_H

#include <HomeDing.h>
#include <ValueElement.h>

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
  virtual bool set(const char *name, const char *value) override;

  /**
   * @brief Give some processing time to the timer to check for next action.
   */
  virtual void loop() override;

private:
  /** The actual active visualized and changeable value */
  int _active = 0;

  /** The number of value elements in the list. */
  int _count = 0;

ValueElement *valueList[MAXMENUVALUES];

  /** Flag for need to update the value. */
  bool _updateV = false;

  /** Flag for need to update the menu. */
  bool _updateM = false;


  /**
   * @brief The _displayAction holds the actions that is submitted when a new menu item was selected or the value has changed.
   */
  String _displayAction;

  /**
   * @brief The _valueAction holds the actions that is submitted when the value has changed.
   */
  String _valueAction;

  /**
   * @brief The _menuAction holds the actions that is submitted when a new menu item was selected.
   */
  String _menuAction;
};

/* ===== Register the Element ===== */

//  should be used to allow the sketch to select the
// available Elements. See <HomeDing.h> the move these lines to MenuElement.h:

#ifdef HOMEDING_REGISTER
// Register the MenuElement onto the ElementRegistry.
bool MenuElement::registered =
    ElementRegistry::registerElement("menu", MenuElement::create);
#endif

#endif