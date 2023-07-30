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

#include <lib/gt911.h>

/**
 * @brief
 */
class DisplayTouchGT911Element : public Element {
public:
  /**
   * @brief Factory function to create a DisplayTouchGT911Element.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;

  /**
   * @brief Construct a new DisplayTouchGT911Element
   */
  DisplayTouchGT911Element();

  /**
   * @brief initialize a new Element.
   * @param board The board reference.
   */
  virtual void init(Board *board) override;

  /**
   * @brief Set a parameter or property to a new value or start an action.
   * @param name Name of property.
   * @param value Value of property.
   * @return true when property could be changed and the corresponding action
   * could be executed.
   */
  virtual bool set(const char *name, const char *value) override;

  /**
   * @brief Start the Element.
   */
  virtual void start() override;

  /**
   * @brief Give some processing time to the timer to check for next action.
   */
  virtual void loop() override;

  /**
   * @brief stop all activities and go inactive.
   */
  virtual void term() override;

  /**
   * @brief push the current value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void pushState(
    std::function<void(const char *pName, const char *eValue)> callback) override;

private:
  // Configuration Properties

  uint16_t _width = 320;
  uint16_t _height = 480;
  int _rotation = 0;

  int _interruptPin = 21;
  int _resetPin = 25;
  
  /// @brief The default I2C address for the touch controller.
  int _address = 0x5D; 


  GT911 *tp = nullptr;

  // found button at first touch position
  uint16_t lastX, lastY;
  DisplayButtonElement *_bFound;

  unsigned long nextRead;
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
