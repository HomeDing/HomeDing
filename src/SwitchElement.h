/**
 * @file SwitchElement.h
 * @brief Element switching a digital signal HIGH LOW based on actions.
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
 * * 29.04.2018 created by Matthias Hertel
 * * 16.09.2018 pullup config added.
 * * 27.11.2018 also can be defined without a physical pin to be controlled by
 * the web ui.
 * * 06.09.2019 derivation of switch element
 */

#ifndef SWITCH_H
#define SWITCH_H

#include <HomeDing.h>
#include "ValueElement.h"

/**
 * @brief The SwitchElement is an special Element that creates actions based on
 * a digital IO signal.
 */
class SwitchElement : public ValueElement
{
public:
  /**
   * @brief Factory function to create a SwitchElement.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;

  /**
   * @brief initialize a new Element.
   * @param board The board reference.
   */
  virtual void init(Board *board) override;


  /**
   * @brief Set a parameter or property to a new value or start an action.
   * @param name Name of property.
   * @param value Value of property.
   * @return true when property could be changed and the corresponding action
   * could be executed.
   */
  virtual bool set(const char *name, const char *value) override;

protected:
  virtual bool _setValue(int val, bool forceAction = false);

private:
  /**
   * @brief The _highAction is emitted when the logical output level is going from
   * LOW to HIGH.
   */
  String _highAction;

  /**
   * @brief The _lowAction is emitted when the logical output level is going from
   * HIGH to LOW.
   */
  String _lowAction;
};

#ifdef HOMEDING_REGISTER
// Register the SwitchElement onto the ElementRegistry.
bool SwitchElement::registered =
    ElementRegistry::registerElement("switch", SwitchElement::create);
#endif

#endif // SWITCH_H