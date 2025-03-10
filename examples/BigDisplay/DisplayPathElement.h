/**
 * @file DisplayPathElement.h
 * @brief Output Element for displaying a path on a display.
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
 * * 05.01.2024 created by Matthias Hertel
 */


#pragma once

#include <displays/DisplayOutputElement.h>

namespace gfxDraw {
  class gfxDrawPathWidget;
}

/**
 * @brief The DisplayPathElement is an Element that allows to display a path based region like a
 *   polygon on the display based on actions.
 *
 * The parameters specify how the information from the action will be displayed.
 */
class DisplayPathElement : public DisplayOutputElement {
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
  virtual bool set(const char *name, const char *value) override;


  /**
   * @brief Draw this output element.
   */
  virtual void draw() override;

private:
  /// @brief The path to be displayed
  String _path;
  bool _needLoad = false;
  bool _undraw = false;

  // rotation angle and center.
  int16_t _scale = 100;
  int16_t _rotation = 0;

  int16_t _centerX = 0;
  int16_t _centerY = 0;

  gfxDraw::gfxDrawPathWidget *dWidget = nullptr;
};

// #ifdef HOMEDING_REGISTER
// Register the DisplayPathElement onto the ElementRegistry.
bool DisplayPathElement::registered =
  ElementRegistry::registerElement("displaypath", DisplayPathElement::create);
// #endif
