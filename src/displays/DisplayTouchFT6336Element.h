/**
 * @file DisplayTouchFT6336Element.h
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * The FT6336 is a 2-Point Capacitive Touch Sensors for panels using the I2c bus for
 * communication. It can deliver Input results every 40 / 10 msec.
 *
 * Changelog:
 * * 27.05.2023 created
 */

#pragma once

#if defined(ESP32)

#include <displays/DisplayTouchElement.h>

/**
 * @brief
 */
class DisplayTouchFT6336Element : public DisplayTouchElement {
public:
  /// @brief Factory function to create a DisplayTouchFT6336Element.
  /// @return Element*
  static Element *create();

  /// @brief static variable to ensure registering in static init phase.
  static bool registered;


  virtual bool pullSensorData() override;

  /// @brief Start the DisplayTouchFT6336 Element.
  virtual void start() override;

private:
  bool _found = false;
};

/* ===== Register the Element ===== */

// When transferred to the HomeDing library a #define like the
// HOMEDING_INCLUDE_DISPLAYTOUCHFT6336 should be used to allow the sketch to select the
// available Elements. See <HomeDing.h> the move these lines to DisplayTouchFT6336Element.h:

#ifdef HOMEDING_REGISTER
// Register the DisplayTouchFT6336Element onto the ElementRegistry.
bool DisplayTouchFT6336Element::registered =
  ElementRegistry::registerElement("displaytouchft6336", DisplayTouchFT6336Element::create);
#endif

#endif
