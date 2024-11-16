/**
 * @file DisplayTouchGT911Element.h
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * The GT911 is a 5-Point Capacitive Touch Sensors for 7" to 8" panels using the I2c bus for
 * communication. It can deliver Input results every 40 / 10 msec.
 * This element implements using the GT911 Touch Controller in "Normal" and "Green" mode.
 *
 * Changelog:
 * * 27.05.2023 created
 */

#pragma once

#if defined(ESP32)

#include <displays/DisplayTouchElement.h>
#include <lib/gt911.h>

/**
 * @brief
 */
class DisplayTouchGT911Element : public DisplayTouchElement {
public:
  /// @brief Factory function to create a DisplayTouchGT911Element.
  /// @return Element*
  static Element *create();

  /// @brief static variable to ensure registering in static init phase.
  static bool registered;


  virtual bool pullSensorData() override;

  /// @brief Start the Element.
  virtual void start() override;

 private:
  // Configuration Properties

  GT911 *tp = nullptr;

};

/* ===== Register the Element ===== */

// When transferred to the HomeDing library a #define like the
// HOMEDING_INCLUDE_DISPLAYTOUCHGT911 should be used to allow the sketch to select the
// available Elements. See <HomeDing.h> the move these lines to DisplayTouchGT911Element.h:

 #ifdef HOMEDING_REGISTER
// Register the DisplayTouchGT911Element onto the ElementRegistry.
bool DisplayTouchGT911Element::registered =
  ElementRegistry::registerElement("displaytouchgt911", DisplayTouchGT911Element::create);
#endif

#endif
