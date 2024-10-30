/**
 * @file DisplayElement.h
 * @brief Implementation of the base function for a DisplayXXXElement.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license.
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:
 * * 29.08.2020 created by Matthias Hertel
 * * 17.03.2022 unified HomeDing::DisplayConfig
 */

#pragma once

#include <functional>

#include <displays/DisplayConfig.h>
#include <displays/BoundingBox.h>

class DisplayAdapter;

#define RGB_WHITE 0x00FFFFFF
#define RGB_BLACK 0x00000000
#define RGB_GRAY 0x00808080
#define RGB_SILVER 0x00C0C0C0
#define RGB_RED 0x00FF0000
#define RGB_GREEN 0x0000FF00
#define RGB_BLUE 0x000000FF
#define RGB_YELLOW 0x00FFFF00
#define RGB_ORANGE 0x00FFA500
#define RGB_PURPLE 0x00800080

#define RGB_UNDEFINED 0xEE000000    // key color
#define RGB_TRANSPARENT 0xEF000000  // key color

#define RGB_IS_COLOR(col) ((col != RGB_UNDEFINED) && (col != RGB_TRANSPARENT))
#define RGB_NO_COLOR(col) ((col == RGB_UNDEFINED) || (col == RGB_TRANSPARENT))


class DisplayElement : public Element {
public:
  /* ===== Element functions ===== */

  /**
   * @brief Constructor of a new DisplayElement.
   */
  DisplayElement();

  /**
   * @brief initialize the DisplayElement.
   * @param board The board reference.
   */
  virtual void init(Board *board);

  /**
   * @brief Set a parameter or property to a new value or start an action.
   * @param name Name of property.
   * @param value Value of property.
   * @return true when property could be changed and the corresponding action
   * could be executed.
   */
  virtual bool set(const char *name, const char *value) override;

  /// @brief Activate the Element.
  virtual void start() override;

  /// @brief Activate the Element using the given adapter.
  virtual void start(DisplayAdapter *displayAdapter);

  // no loop() call required for display elements as they are used for configuration only.

  /**
   * @brief push the current value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void pushState(
    std::function<void(const char *pName, const char *eValue)> callback) override;

  // === common properties for initializing display adapters

protected:
  // HomeDing::DisplayConfig *config;

  /** event when page changes */
  String _onPage;

private:
  void _newPage(int page);
};

// This is a base class that will not be registered
