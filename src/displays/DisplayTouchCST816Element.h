/**
 * @file DisplayTouchCST816Element.h
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * The CST816 is a 1-Point Capacitive Touch Sensors for small panels using the I2c bus for
 * communication.
 * This element implements using the CST816 Touch Controller for touch events detection.
 *
 * Changelog:
 * * 17.11.2023 created
 */

#pragma once

#include <displays/DisplayTouchElement.h>
#include <lib/cst816.h>

/**
 * @brief
 */
class DisplayTouchCST816Element : public DisplayTouchElement {
public:
  /// @brief Factory function to create a DisplayTouchCST816Element.
  /// @return Element*
  static Element *create();

  /// @brief static variable to ensure registering in static init phase.
  static bool registered;

  virtual bool pullSensorData() override;

  /// @brief Start the Element.
  virtual void start() override;

private:
  // Configuration Properties
  CST816 *otp = nullptr;
};

/* ===== Register the Element ===== */

// When transferred to the HomeDing library a #define like the
// HOMEDING_INCLUDE_DISPLAYTOUCHCST816 should be used to allow the sketch to select the
// available Elements. See <HomeDing.h> the move these lines to DisplayTouchCST816Element.h:

#ifdef HOMEDING_REGISTER
// Register the DisplayTouchCST816Element onto the ElementRegistry.
bool DisplayTouchCST816Element::registered =
  ElementRegistry::registerElement("displaytouchcst816", DisplayTouchCST816Element::create);
#endif
