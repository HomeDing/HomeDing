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
 * * 26.01.2024 force redraw on attribute changes
 * * 20.02.2024 added common value handling
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

  // Constructor
  DisplayOutputElement();

  /// @brief Set a parameter or property to a new value or start an action.
  /// @param name Name of the property.
  /// @param value Value of the property.
  /// @return true when property was set or the action was received.
  virtual bool set(const char *name, const char *value) override;

  /// @brief Activate the DisplayOutputElement.
  /// @return true when activation was good.
  /// @return false when activation failed.
  virtual void start() override;

  /// @brief draw the element on the display adapter.
  virtual void draw();

  /// @brief push the current value of all properties to the callback.
  /// @param callback callback function that is used for every property.
  virtual void pushState(
    std::function<void(const char *pName, const char *eValue)> callback);

  /// @brief Bounding Box
  BoundingBox box;

  /// @brief Redraw needed flag;
  bool needsDraw = false;

  /// @brief Page of the display where the element is placed. Default on page 1.
  int page = 1;

protected:
  /// @brief Draw color of the element
  uint32_t _color = RGB_UNDEFINED;

  /// @brief Background color of the element
  uint32_t _backgroundColor = RGB_UNDEFINED;

  /// @brief Border color of the element
  uint32_t _borderColor = RGB_UNDEFINED;

  /// @brief Value the element
  String _value;

  /// @brief Reference to DisplayAdapter
  DisplayAdapter *_display = nullptr;
};
