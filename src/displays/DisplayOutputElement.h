/**
 * @file displays/DisplayOutputElement.h
 * @brief Base Output Element for a display.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 *
 * More information on https://www.mathertel.de/Arduino.
 *
 * Changelog:
 * * 26.04.2021 created by Matthias Hertel
 */


#pragma once

#include <displays/DisplayAdapter.h>

/**
 * @brief The DisplayTextElement is an Element that allows to create information
 * on the display based on actions.
 *
 * The parameters specify how the information from the action will be displayed.
 */
class DisplayOutputElement : public Element {
public:
  /// @brief Set a parameter or property to a new value or start an action.
  /// @param name Name of the property.
  /// @param value Value of the property.
  /// @return true when property was set or the action was received.
  virtual bool set(const char *name, const char *value) override;

  /// @brief Activate the DisplayOutputElement.
  /// @return true when activation was good.
  /// @return false when activation failed.
  virtual void start() override;

  /**
   * @brief Give some processing time to do drawing if required.
   */
  virtual void loop() override;

  /// @brief draw the element on the display adapter.
  virtual void draw();


protected:
  /// @brief return true when object is at the specified position or is overlapping with rectangle
  bool overlap(int16_t rx, int16_t ry, uint16_t rw = 0, uint16_t rh = 0);

  /// @brief Page of the display where the element is placed. Default on page 1.
  int _page = 1;

  /// @brief X-position ot the output element
  uint16_t _x;

  /// @brief Y-position ot the output element
  uint16_t _y;

  /// @brief Width ot the output element
  uint16_t _w = 100;

  /// @brief height and fontsize
  uint16_t _h = 10;

  /// @brief Draw color of the element
  uint32_t _color = RGB_UNDEFINED;

  /// @brief Background color of the element
  uint32_t _backgroundColor = RGB_UNDEFINED;

  /// @brief Border color of the element
  uint32_t _borderColor = RGB_UNDEFINED;

  /// @brief Reference to DisplayAdapter
  DisplayAdapter *_display = nullptr;

  /// @brief Redraw needed flag;
  bool _needredraw = false;
};
