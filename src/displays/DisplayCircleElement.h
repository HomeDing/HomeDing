/**
 * @file DisplayCircleElement.h
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
 * * 29.04.2018 created by Matthias Hertel
 * * 24.06.2018 no problems when no display is available.
 */


#pragma once

#if defined(ESP32)

#include <displays/DisplayOutputElement.h>

/**
 * @brief The DisplayCircleElement is an Element that allows to create information
 * on the display based on actions.
 *
 * The parameters specify how the information from the action will be displayed.
 */
class DisplayCircleElement : public DisplayOutputElement {
public:
  /// @brief Factory function to create a ButtonElement.
  /// @return Element*
  static Element *create();

  /// @brief static variable to ensure registering in static init phase.
  static bool registered;


  /// @brief Draw this output element.
  virtual void draw() override;
};

#ifdef HOMEDING_REGISTER
// Register the DisplayCircleElement onto the ElementRegistry.
bool DisplayCircleElement::registered =
  ElementRegistry::registerElement("displaycircle", DisplayCircleElement::create);
#endif

#endif
