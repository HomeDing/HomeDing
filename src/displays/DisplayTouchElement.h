/**
 * @file DisplayTouchElement.h
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * This class implements the base functionality for display touch controllers.
 *
 * Changelog:
 * * 25.10.2023 created
 * * 17.11.2023 rotation support 
 */

#pragma once

#include <displays/DisplayButtonElement.h>

/**
 * @brief
 */
class DisplayTouchElement : public Element {
public:

  /// @brief Construct a new DisplayTouchElement
  DisplayTouchElement();


  virtual bool pullSensorData();

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

  /// @brief Start the Element.
  virtual void start() override;

  /**
   * @brief Give some processing time to the timer to check for next action.
   */
  virtual void loop() override;

protected:
  // Configuration Properties

  /// @brief width of the touch area - same as the display
  uint16_t _width = 320;

  /// @brief height of the touch area - same as the display
  uint16_t _height = 480;

  /// @brief rotation of the display
  int _rotation = 0;

  /// @brief pin for interrupt signal from the touch chip
  int _interruptPin = -1; // must be configured

  /// @brief reset pin for interrupt signal from the touch chip
  int _resetPin = -1; // must be configured
  
  /// @brief The default I2C address for the touch controller.
  int _address = 0x5D; 

  // position of last touch event
  uint16_t lastX, lastY;

  // found button at first touch position
  DisplayButtonElement *_bFound;

  unsigned long nextRead;
};
