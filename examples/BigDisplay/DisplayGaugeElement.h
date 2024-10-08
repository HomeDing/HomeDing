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
  float _value, _lastValue;

  /// total gauge segments angle
  int16_t _degree = 280;

  /// outer radius of the gauge segments
  int16_t _radiusOut;

  /// inner radius of the gauge segments
  int16_t _radiusIn;

  /// minimum and maximum of values
  int16_t _minValue = 0;
  int16_t _maxValue = 100;

  /// @brief draw a gauge range segment
  /// @param a0 starting angle
  /// @param a1 ending angle
  /// @param fillcolor color of range segment
  void _drawRangeSegment(int16_t a0, int16_t a1, uint32_t fillcolor);

};

// #ifdef HOMEDING_REGISTER
// Register the DisplayGaugeElement onto the ElementRegistry.
bool DisplayGaugeElement::registered =
  ElementRegistry::registerElement("displaygauge", DisplayGaugeElement::create);
// #endif
