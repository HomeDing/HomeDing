// -----
// Display.h - Library to output an digital signals based on actions.
//
// Copyright (c) by Matthias Hertel, https://www.mathertel.de.
//
// This work is licensed under a BSD style license.
// See https://www.mathertel.de/License.aspx.
// More information on https://www.mathertel.de/Arduino
// -----
// 29.04.2018 created by Matthias Hertel
// 24.06.2018 no problems when no display is available.
// -----


#ifndef DISPLAYELEMENT_H
#define DISPLAYELEMENT_H

#include <Arduino.h>

#include "Board.h"
#include "Element.h"

#include "DisplayAdapter.h"

#define DISPLAY_TYPE_TEXT 0x01
#define DISPLAY_TYPE_DOT 0x02

/**
 * @brief The DisplayElement is an Element that allows to create information on
 * the display based on actions.
 *
 * The parameters specify how the information from the action will be displayed.
 */
class DisplayElement : public Element
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
   * @brief Activate the DisplayElement.
   * @return true when activation was good.
   * @return false when activation failed.
   */
  virtual void start();

  /**
   * @brief push the current value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void
  pushState(std::function<void(const char *pName, const char *eValue)> callback);


private:
  /**
   * @brief This variable corresponds to the type parameter.
   */
  int _type = 0;

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

#endif // DISPLAYELEMENT_H