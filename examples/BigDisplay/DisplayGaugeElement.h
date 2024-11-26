/**
 * @file DisplayGaugeElement.h
 * @brief Output Element for displaying a gauge with a value on a display.
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
 * * 14.08.2024 created by Matthias Hertel
 */


#pragma once

#include <displays/DisplayOutputElement.h>

namespace gfxDraw {
  class gfxDrawGaugeWidget;
  class GFXDrawGaugeConfig;
}


/**
 * @brief The DisplayGaugeElement is an Element that allows to display a path based region like a
 *   polygon on the display based on actions.
 *
 * The parameters specify how the information from the action will be displayed.
 */
class DisplayGaugeElement : public DisplayOutputElement {
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


  /// @brief Constructor
  DisplayGaugeElement();


  /// @brief Activate the Element.
  virtual void start() override;


  /// @brief Set a parameter or property to a new value or start an action.
  /// @param name Name of the property.
  /// @param value Value of the property.
  /// @return true when property was set or the action was received.
  virtual bool set(const char *name, const char *value) override;


  /**
   * @brief Draw this output element.
   */
  virtual void draw() override;


private:

  /// total gauge segments angle
  int16_t _startDegree = 30;
  int16_t _endDegree = 330;

  /// minimum and maximum of values
  int16_t _minValue = 0;
  int16_t _maxValue = 100;

  gfxDraw::gfxDrawGaugeWidget *_gWidget = nullptr;
  gfxDraw::GFXDrawGaugeConfig *_gConfig = nullptr;
};

// #ifdef HOMEDING_REGISTER
// Register the DisplayGaugeElement onto the ElementRegistry.
bool DisplayGaugeElement::registered =
  ElementRegistry::registerElement("displaygauge", DisplayGaugeElement::create);
// #endif
