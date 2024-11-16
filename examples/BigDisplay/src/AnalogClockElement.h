/**
 * @file AnalogClockElement.h
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:
 * * 30.01.2022 created by Matthias Hertel
 */

#pragma once

#include <displays/DisplayOutputElement.h>

/**
 * @brief
 */
class AnalogClockElement : public DisplayOutputElement {
public:
  /**
   * @brief Factory function to create a AnalogClockElement.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;

  /**
   * @brief Construct a new AnalogClockElement
   */
  AnalogClockElement();

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

  virtual void draw() override;

  /**
   * @brief stop all activities and go inactive.
   */
  virtual void term() override;


private:
  time_t _shown_time = 0;

  // colors
  // uint32_t _color = RGB_GRAY; // provided by DisplayOutputElement
  uint32_t _hand_color_s = RGB_GRAY;
  uint32_t _hand_color_m = RGB_WHITE;
  uint32_t _hand_color_h = RGB_WHITE;

  // last drawn angles of hands
  uint16_t _last_a_sec;
  uint16_t _last_a_min;
  uint16_t _last_a_hour;

  uint16_t _radius = 100;
  uint16_t _cx = 48;
  uint16_t _cy = 48;

  void _drawClock();
  void _drawHand(uint16_t deg, uint16_t len, uint16_t width, uint32_t color);


  /**
   * @brief The _xAction holds the actions that is submitted when ...
   */
  String _xAction;
};

/* ===== Register the Element ===== */

#ifdef HOMEDING_REGISTER
// Register the AnalogClockElement in the ElementRegistry.
bool AnalogClockElement::registered =
  ElementRegistry::registerElement("analogclock", AnalogClockElement::create);
#endif
