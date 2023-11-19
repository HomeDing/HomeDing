/**
 * @file DisplayLineElement.h
 * @brief Output Element for controlling a binary output on a display.
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
 * @brief The DisplayLineElement is an Element that allows to create information
 * on the display based on actions.
 *
 * The parameters specify how the information from the action will be displayed.
 */
class DisplayLineElement : public DisplayOutputElement
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

  /**
   * @brief Draw this output element.
   */
  virtual void draw() override;

private:
  /**
   * @brief This variable corresponds to the x0 parameter.
   */
  uint16_t _x1 = 0;
  uint16_t _y1 = 0;
};

#ifdef HOMEDING_REGISTER
// Register the DisplayLineElement onto the ElementRegistry.
bool DisplayLineElement::registered =
    ElementRegistry::registerElement("displayline", DisplayLineElement::create);
#endif
