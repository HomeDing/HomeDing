/**
 * @file DisplayRectElement.h
 * @brief Output Element for controlling a binary output on a display.
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
 * * 15.02.2018 created by Matthias Hertel
 */


#pragma once

#if defined(ESP32)

#include <displays/DisplayOutputElement.h>

/**
 * @brief The DisplayRectElement is an Element that allows to create information
 * on the display based on actions.
 *
 * The parameters specify how the information from the action will be displayed.
 */
class DisplayRectElement : public DisplayOutputElement {
public:
  /// @brief static factory function to create a new DisplayRectElement.
  /// @return DisplayRectElement* as Element* created element.
  static Element *create();

  /// @brief static variable to ensure registering in static init phase.
  static bool registered;

  /// @brief Draw the rectangle
  virtual void draw() override;

  /// @brief Draw the rectangle
  virtual void draw(bool useBackground);

private:
};

#ifdef HOMEDING_REGISTER
// Register the DisplayRectElement onto the ElementRegistry.
bool DisplayRectElement::registered =
  ElementRegistry::registerElement("displayrect", DisplayRectElement::create);
#endif

#endif
