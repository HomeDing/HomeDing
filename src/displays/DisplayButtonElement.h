/**
 * @file displays/DisplayButtonElement.h
 * @brief Output Element for displaying a button on a display.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license,
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino.
 *
 * Changelog:
 * * 29.04.2018 created by Matthias Hertel
 * * 24.06.2018 no problems when no display is available.
 * * 23.09.2018 support DisplayAdapter->flush()
 */


#pragma once

#include <displays/DisplayOutputElement.h>

/**
 * @brief The DisplayButtonElement is an Element that allows to create information
 * on the display based on actions.
 *
 * The parameters specify how the information from the action will be displayed.
 */
class DisplayButtonElement : public DisplayOutputElement {
public:
  /**
   * @brief Factory function to create a ButtonElement.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;


  /**
   * @brief Set a parameter or property to a new value or start an action.
   * @param name Name of property.
   * @param value Value of property.
   * @return true when property could be changed and the corresponding action
   * could be executed.
   */
  virtual bool set(const char *name, const char *value) override;


  /// @brief Initialize the DisplayButtonElement.
  /// @param board Reference to the board.
  virtual void init(Board *board) override;


  // ===== Display Output supporting functions =====

  /// @brief Draw the button on display.
  /// used by DisplayOutputElement.
  virtual void draw() override;


  // ===== Touch supporting functions =====

  /// @brief A touchStart event is given
  /// @param x X position of touch start event.
  /// @param y Y position of touch start event.
  /// @return true when button wants to participate in further actions.
  bool touchStart(uint16_t xPos, uint16_t yPos);

  /// @brief A touchEnd event is given
  void touchEnd(uint16_t xPos, uint16_t yPos);

private:
  /// @brief button is pressed.
  bool _pressed;

  /// @brief button click event dispatched on button release.
  String _clickAction;

  /**
   * @brief displayed value
   */
  String _text;
};

#ifdef HOMEDING_REGISTER
// Register the DisplayButtonElement onto the ElementRegistry.
bool DisplayButtonElement::registered =
  ElementRegistry::registerElement("displaybutton", DisplayButtonElement::create);
#endif
