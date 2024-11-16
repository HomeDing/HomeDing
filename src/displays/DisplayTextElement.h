/**
 * @file DisplayTextElement.h
 * @brief Output Element for controlling a text output on a display.
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
 * * 26.01.2024 force redraw on attribute changes
 * * 16.11.2024 independant fontsize from textbox, horizontal alignment implemented
 */


#pragma once

#include <displays/DisplayAdapter.h>
#include <displays/DisplayOutputElement.h>

/**
 * @brief The DisplayTextElement is an Element that allows to create information
 * on the display based on actions.
 *
 * The parameters specify how the information from the action will be displayed.
 */
class DisplayTextElement : public DisplayOutputElement
{
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

  /// @brief Activate the DisplayTextElement.
  /// @return true when activation was good.
  /// @return false when activation failed.
  virtual void start() override;

protected:
  /// @brief send current text to the display.
  virtual void draw() override;

private:
  /// @brief When using the TEXT type this text is shown before the value text.
  String _prefix; // static text before the value

  /// @brief When using the TEXT type this text is shown after the value text.
  String _postfix; //  after the value
};

#ifdef HOMEDING_REGISTER
// Register the DisplayTextElement onto the ElementRegistry.
bool DisplayTextElement::registered =
    ElementRegistry::registerElement("displaytext", DisplayTextElement::create);
#endif
