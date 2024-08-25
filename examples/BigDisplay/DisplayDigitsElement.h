/**
 * @file DisplayDigitsElement.h
 * @brief Output Element for displaying 7 segment digits on a display.
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
 * * 14.01.2024 created by Matthias Hertel
 */


#pragma once

#include <displays/DisplayOutputElement.h>
#include <gfxDraw.h>
#include <gfxDrawWidget.h>

/**
 * @brief The DisplayDigitsElement is an Element that allows to display a path based region like a
 *   polygon on the display based on actions.
 *
 * The parameters specify how the information from the action will be displayed.
 */
class DisplayDigitsElement : public DisplayOutputElement {
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


  DisplayDigitsElement();

  /**
   * @brief Set a parameter or property to a new value or start an action.
   * @param name Name of property.
   * @param value Value of property.
   * @return true when property could be changed and the corresponding action
   * could be executed.
   */
  virtual bool set(const char *name, const char *value) override;


  /**
   * @brief Draw this output element.
   */
  virtual void draw() override;

private:
  String _lastValue;

  /// scaling factor of the paths in percent
  int16_t _scale = 300;

  void _drawDigit(const char *path, int16_t x, int16_t y);
  void _drawDigits7(char ch, int16_t x, int16_t y);
};

// #ifdef HOMEDING_REGISTER
// Register the DisplayDigitsElement onto the ElementRegistry.
bool DisplayDigitsElement::registered =
  ElementRegistry::registerElement("displaydigits", DisplayDigitsElement::create);
// #endif
