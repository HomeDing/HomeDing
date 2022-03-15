/**
 * @file DisplayOutputElement.h
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


#ifndef DisplayOutputElement_H
#define DisplayOutputElement_H

#include <HomeDing.h>

#include <displays/DisplayAdapter.h>

#define COLOR_UNDEFINED 0xEE000000  //

/**
 * @brief The DisplayTextElement is an Element that allows to create information
 * on the display based on actions.
 *
 * The parameters specify how the information from the action will be displayed.
 */
class DisplayOutputElement : public Element {
public:
  /**
   * @brief Set a parameter or property to a new value or start an action.
   * @param name Name of property.
   * @param value Value of property.
   * @return true when property could be changed and the corresponding action
   * could be executed.
   */
  virtual bool set(const char *name, const char *value) override;

  /**
   * @brief Activate the DisplayTextElement.
   * @return true when activation was good.
   * @return false when activation failed.
   */
  virtual void start() override;

  /**
   * @brief Give some processing time to do drawing if required.
   */
  virtual void loop() override;

  /**
   * @brief send element to display
   */
  virtual void draw();

protected:
  /**
   * @brief Page of the display where the element is placed.
   */
  int _page = 1;  // Put on page 1 by default.

  int _x;  ///< x-position
  int _y;  ///< x-position

  uint32_t _color = COLOR_UNDEFINED;  ///< element color

  // Reference to DisplayAdapter
  DisplayAdapter *_display = nullptr;

  /**
   * @brief Redraw needed flag;
   */
  bool _neededraw = false;
};

#endif  // DisplayOutputElement_H