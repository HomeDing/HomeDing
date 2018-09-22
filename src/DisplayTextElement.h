/**
 * @file DisplayTextElement.h
 * @brief Output Element for controlling a text output on a display.
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
 * * 24.06.2018 no problems when no display is available.
 */


#ifndef DisplayTextElement_H
#define DisplayTextElement_H

#include <Arduino.h>
#include "Board.h"
#include "Element.h"

#include <displays/DisplayAdapter.h>

/**
 * @brief The DisplayTextElement is an Element that allows to create information
 * on the display based on actions.
 *
 * The parameters specify how the information from the action will be displayed.
 */
class DisplayTextElement : public Element
{
public:
  /**
   * @brief Factory function to create a ButtonElement.
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
   * @brief Activate the DisplayTextElement.
   * @return true when activation was good.
   * @return false when activation failed.
   */
  virtual void start();

  /**
   * @brief push the current value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void pushState(
      std::function<void(const char *pName, const char *eValue)> callback);


private:
  /**
   * @brief When using the TEXT type this text is shown before the value text.
   */
  char _prefix[MAX_ID_LENGTH]; // static text before the value

  /**
   * @brief When using the TEXT type this text is shown after the value text.
   */
  char _postfix[MAX_ID_LENGTH]; //  after the value

  /**
   * @brief displayed value
   */
  String _value;

  /**
   * @brief This variable corresponds to the x parameter.
   */
  int _x = 0;

  /**
   * @brief This variable corresponds to the y parameter.
   */
  int _y = 0;

  int _w = 100;
  int _h = 10; // also used as _fontsize

  DisplayAdapter *_display = NULL;
};

#ifdef HOMEDING_REGISTER
// Register the DisplayTextElement onto the ElementRegistry.
bool DisplayTextElement::registered =
    ElementRegistry::registerElement("displaytext", DisplayTextElement::create);
#endif

#endif // DisplayTextElement_H